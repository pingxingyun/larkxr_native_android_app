package com.pxy.cloudlarkxroculus.Activity;

import static android.content.Intent.FLAG_ACTIVITY_SINGLE_TOP;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.DataSetObserver;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.Process;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.SpinnerAdapter;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.widget.SwitchCompat;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.alibaba.fastjson.JSON;
import com.pxy.cloudlarkxrkit.Config;
import com.pxy.cloudlarkxroculus.Adapter.AppListAdapter;
import com.pxy.cloudlarkxroculus.Bean.ServerBean;
import com.pxy.cloudlarkxroculus.MainActivity;
import com.pxy.cloudlarkxroculus.R;
import com.pxy.cloudlarkxroculus.UI.AnimatBanner;
import com.pxy.larkcore.ClientLifeManager;
import com.pxy.larkcore.CloudlarkManager;
import com.pxy.larkcore.ImSocketChannel;
import com.pxy.larkcore.SocketChannelObserver;
import com.pxy.larkcore.Util;
import com.pxy.larkcore.request.AppListItem;
import com.pxy.larkcore.request.Base;
import com.pxy.larkcore.request.Bean.GetRunModeBean;
import com.pxy.larkcore.request.GetAppliList;
import com.pxy.larkcore.request.GetRunMode;
import com.pxy.larkcore.request.PageInfo;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

public class ListActivity extends Activity {
    private static final String TAG = "pvr_activity_list";
    private static final String SETTING = "pxy_setting";
    private static final String SETTING_LIST_3D = "list3D";
    //网络设置
    private static final String SETTING_SERVER = "serverAddress";
    private static final String SETTING_SERVER_USE_HTTPS = "useHttps";
    private List<ServerBean> serverBeanList = new ArrayList<>();
    private Spinner serverList;
    //生命周期管理
    private ClientLifeManager clientLifeManager;
    private Config config = null;
    //IP
    private String mServerIp = "";
    private boolean useHttps;

    //设置按钮和列表
    private Button confirmip, closeip, closeSetTab, clearServer;
    private RecyclerView rec;
    //设置面板
    private LinearLayout setIp, advancedList, setTab, selfOnline;
    private EditText inputIp, inputPort;
    //列表adapter
    private AppListAdapter appListAdapter;
    //列表组件
    private SwitchCompat advancedSetting, list3D, vibrator, UseH265, reportFecFailed, UseFovRending, Use10Bit;
    private RadioGroup QuickConfigLevel;
    private RadioButton QuickConfigLevel_Manual, QuickConfigLevel_Auto, QuickConfigLevel_Fast, QuickConfigLevel_Normal, QuickConfigLevel_Extreme;
    private SeekBar resolutionScaleBar, coderateBar;
    private TextView resolutionScale, coderate, SelfOnlineText;
    private RadioGroup StreamType;
    private RadioButton larkStreamType_UDP, larkStreamType_TCP, larkStreamType_THROTTLED_UDP;
    //socket链接
    private ImSocketChannel imSocketChannel;
    //getrunmode接口开关
    private Boolean stoprunmode = false;
    //翻页请求
    private GetAppliList getAppliList;
    List<AppListItem> applist;
    private int mPage = 1;
    private ImageView lastPage, nextPage;

    //关闭app按钮
    private ImageView closeApp;
    //打开菜单
    private ImageView openMenu, opennet;
    //getrunmode
    private GetRunMode getRunMode;
    //初始引导步骤
    private TextView text1, text2, text3, text4, text5;
    private ConstraintLayout firstrun;
    private int stap = 0;
    //列表样式
    private RadioGroup list_show_type;
    private RadioButton type_list, type_scroll;
    private int list_show_type_num = 0;
    //animateBanner
    private AnimatBanner animateBanner;
    private LinearLayout list_background;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        BaseApplication.getInstance().setmHandler(handler);
        setContentView(R.layout.activity_list);
        FindViewById();

        SharedPreferences sp = getSharedPreferences("firststap", Context.MODE_PRIVATE);
        if (sp.getBoolean("first", true)) {
            firstrun.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (stap >= 5) {
                        stap = 0;
                        SharedPreferences sp = getSharedPreferences("firststap", Context.MODE_PRIVATE);
                        SharedPreferences.Editor editor = sp.edit();
                        editor.putBoolean("first", false);
                        editor.apply();
                        firstrun.setVisibility(View.GONE);

                        Init();
                        initview();
                    }
                    StapText(stap++);
                }
            });
            firstrun.setVisibility(View.VISIBLE);
        } else {
            Init();
            initview();
        }

        Log.e("mac", Util.getLocalMacAddress(ListActivity.this));
    }

    public Handler handler = new Handler() {
        public void handleMessage(Message msg) {
            getMessage(msg);
        }
    };

    private void Init() {
        config = Config.readFromCache(this);

        SharedPreferences sp = getSharedPreferences(SETTING, Context.MODE_PRIVATE);
        boolean list3Dbool = sp.getBoolean(SETTING_LIST_3D, false);
        list3D.setChecked(list3Dbool);
        if (list3Dbool) {
            GoMainActivity(ListActivity.this, null);
        }
        if (!sp.getString(SETTING_SERVER, "").isEmpty()) {
            serverBeanList = JSON.parseArray(sp.getString(SETTING_SERVER, ""), ServerBean.class);
            mServerIp = "http://" + serverBeanList.get(0).getIp() + ":" + serverBeanList.get(0).getPort();
            useHttps = serverBeanList.get(0).getUse_https();
            Log.e("getmessage", "init");
            setIp(serverBeanList, 0);
        } else {
            showSetupIP();
        }

        CloudlarkManager.init(this, CloudlarkManager.APP_TYPE_VR);

        ListActivity.this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                switch (config.quickConfigLevel) {
                    case Config.QuickConfigLevel_Manual: {
                        QuickConfigLevel.check(R.id.QuickConfigLevel_Manual);
                        break;
                    }
                    case Config.QuickConfigLevel_Auto: {
                        QuickConfigLevel.check(R.id.QuickConfigLevel_Auto);
                        break;
                    }
                    case Config.QuickConfigLevel_Normal: {
                        QuickConfigLevel.check(R.id.QuickConfigLevel_Normal);
                        break;
                    }
                    case Config.QuickConfigLevel_Fast: {
                        QuickConfigLevel.check(R.id.QuickConfigLevel_Fast);
                        break;
                    }
                    case Config.QuickConfigLevel_Extreme: {
                        QuickConfigLevel.check(R.id.QuickConfigLevel_Extreme);
                        break;
                    }
                }
                vibrator.setChecked(config.vibrator);

                Log.e("resulutionScale", config.resulutionScale + "");

                int resulutionScaleint = (int) (config.resulutionScale * 100);
                resolutionScale.setText(resulutionScaleint + "");
                coderate.setText(config.biteRate + "");

                UseH265.setChecked(config.useH265);
                reportFecFailed.setChecked(config.reportFecFailed);
                UseFovRending.setChecked(config.useFovRendering);
                Use10Bit.setChecked(config.use10BitEncoder);

                resolutionScaleBar.setProgress(resulutionScaleint);
                coderateBar.setProgress(config.biteRate / 10000);
            }
        });
    }

    private void FindViewById() {
        rec = findViewById(R.id.rec);
/*        int screenWidth = getWindowManager().getDefaultDisplay().getWidth(); // 屏幕宽（像素，如：480px）
        int screenHeight = getWindowManager().getDefaultDisplay().getHeight(); // 屏幕高（像素，如：800p）
        int spancount=screenWidth>screenHeight?4:2;*/
        rec.setLayoutManager(new GridLayoutManager(this, 4));
        selfOnline = findViewById(R.id.SelfOnline);

        setTab = findViewById(R.id.setTab);
        setIp = findViewById(R.id.setIP);
        inputIp = findViewById(R.id.inputIp);
        inputPort = findViewById(R.id.inputPort);
        confirmip = findViewById(R.id.confirmip);
        closeip = findViewById(R.id.closeip);
        closeSetTab = findViewById(R.id.closeSetTab);

        advancedSetting = findViewById(R.id.advancedSetting);
        list3D = findViewById(R.id.list3D);
        advancedList = findViewById(R.id.advancedList);
        vibrator = findViewById(R.id.vibrator);

        QuickConfigLevel = findViewById(R.id.QuickConfigLevel);
        QuickConfigLevel_Manual = findViewById(R.id.QuickConfigLevel_Manual);
        QuickConfigLevel_Auto = findViewById(R.id.QuickConfigLevel_Auto);
        QuickConfigLevel_Fast = findViewById(R.id.QuickConfigLevel_Fast);
        QuickConfigLevel_Normal = findViewById(R.id.QuickConfigLevel_Normal);
        QuickConfigLevel_Extreme = findViewById(R.id.QuickConfigLevel_Extreme);

        resolutionScaleBar = findViewById(R.id.resolutionScaleBar);
        resolutionScale = findViewById(R.id.resolutionScale);
        coderateBar = findViewById(R.id.coderateBar);
        coderate = findViewById(R.id.coderate);

        SelfOnlineText = findViewById(R.id.SelfOnlineText);

        StreamType = findViewById(R.id.StreamType);
        larkStreamType_UDP = findViewById(R.id.larkStreamType_UDP);
        larkStreamType_TCP = findViewById(R.id.larkStreamType_TCP);
        larkStreamType_THROTTLED_UDP = findViewById(R.id.larkStreamType_THROTTLED_UDP);

        UseH265 = findViewById(R.id.UseH265);
        reportFecFailed = findViewById(R.id.reportFecFailed);
        UseFovRending = findViewById(R.id.UseFovRending);
        Use10Bit = findViewById(R.id.Use10Bit);

        lastPage = findViewById(R.id.lastPage);
        nextPage = findViewById(R.id.nextPage);

        closeApp = findViewById(R.id.closeApp);
        openMenu = findViewById(R.id.openMenu);
        opennet = findViewById(R.id.opennet);

        serverList = findViewById(R.id.serverList);
        clearServer = findViewById(R.id.clearServer);

        text1 = findViewById(R.id.text1);
        text2 = findViewById(R.id.text2);
        text3 = findViewById(R.id.text3);
        text4 = findViewById(R.id.text4);
        text5 = findViewById(R.id.text5);
        firstrun = findViewById(R.id.firstRun);

        list_show_type = findViewById(R.id.list_show_type);
        animateBanner = findViewById(R.id.animateBanner);
        list_background = findViewById(R.id.list_background);
    }

    private void initview() {
        Log.d(TAG, "cached server address use https " + useHttps + " serverIp " + mServerIp);
        opennet.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showSetupIP();
            }
        });
        closeip.setOnClickListener(v -> setIp.setVisibility(View.GONE));

        openMenu.setOnClickListener(v -> {
            Init();
            setTab.setVisibility(View.VISIBLE);
        });

        confirmip.setOnClickListener(v -> closeSetupIP());

        advancedSetting.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                advancedList.setVisibility(View.VISIBLE);
            } else {
                advancedList.setVisibility(View.GONE);
            }
        });

        list3D.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                SharedPreferences sp = ListActivity.this.getSharedPreferences(SETTING, Context.MODE_PRIVATE);
                SharedPreferences.Editor editor = sp.edit();
                editor.putBoolean(SETTING_LIST_3D, isChecked);
                editor.apply();
            }
        });

        closeSetTab.setOnClickListener(v -> {
            setTab.setVisibility(View.GONE);
            Config.saveToCache(ListActivity.this, config);
            if (list3D.isChecked()) {
                GoMainActivity(ListActivity.this, null);
            }
        });

        QuickConfigLevel.setOnCheckedChangeListener((group, checkedId) -> {
            switch (checkedId) {
                case R.id.QuickConfigLevel_Manual: {
                    config.quickConfigLevel = Config.QuickConfigLevel_Manual;
                    break;
                }
                case R.id.QuickConfigLevel_Auto: {
                    config.quickConfigLevel = Config.QuickConfigLevel_Auto;
                    break;
                }
                case R.id.QuickConfigLevel_Fast: {
                    config.quickConfigLevel = Config.QuickConfigLevel_Fast;
                    break;
                }
                case R.id.QuickConfigLevel_Normal: {
                    config.quickConfigLevel = Config.QuickConfigLevel_Normal;
                    break;
                }
                case R.id.QuickConfigLevel_Extreme: {
                    config.quickConfigLevel = Config.QuickConfigLevel_Extreme;
                    break;
                }
            }
        });

        vibrator.setOnCheckedChangeListener((buttonView, isChecked) -> {
            config.vibrator = isChecked;
        });

        resolutionScaleBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                ListActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        /*DecimalFormat df=new DecimalFormat("0.00");
                        resolutionScale.setText(df.format((float)progress/100));*/
                        resolutionScale.setText(progress + "");
                        config.resulutionScale = (float) progress / 100;
                    }
                });
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        coderateBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                ListActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        coderate.setText(progress * 10000 + "");
                        config.biteRate = progress * 10000;
                    }
                });
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        StreamType.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                switch (checkedId) {
                    case R.id.larkStreamType_UDP: {
                        config.streamType = Config.larkStreamType_UDP;
                        break;
                    }
                    case R.id.larkStreamType_TCP: {
                        config.streamType = Config.larkStreamType_TCP;
                        break;
                    }
                    case R.id.larkStreamType_THROTTLED_UDP: {
                        config.streamType = Config.larkStreamType_THROTTLED_UDP;
                        break;
                    }
                }
            }
        });

        UseH265.setOnCheckedChangeListener((buttonView, isChecked) -> {
            config.useH265 = isChecked;
        });

        reportFecFailed.setOnCheckedChangeListener((buttonView, isChecked) -> {
            config.reportFecFailed = isChecked;
        });

        UseFovRending.setOnCheckedChangeListener((buttonView, isChecked) -> {
            config.useFovRendering = isChecked;
        });

        Use10Bit.setOnCheckedChangeListener((buttonView, isChecked) -> {
            config.use10BitEncoder = isChecked;
        });

        lastPage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (getAppliList != null) {
                    getAppliList.setPage(--mPage);
                    getAppliList.getAppliList();
                }
            }
        });

        nextPage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (getAppliList != null) {
                    getAppliList.setPage(++mPage);
                    getAppliList.getAppliList();
                }
            }
        });

        closeApp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                StopApp(true);
            }
        });

        serverList.setAdapter(new SpinnerAdapter() {
            @Override
            public View getDropDownView(int position, View convertView, ViewGroup parent) {
                if (convertView == null) {
                    convertView = LayoutInflater.from(ListActivity.this).inflate(R.layout.spinner_item, null);
                    TextView textView = convertView.findViewById(R.id.text);
                    textView.setText("http://" + serverBeanList.get(position).getIp() + ":" + serverBeanList.get(position).getPort());
                }
                return convertView;
            }

            @Override
            public void registerDataSetObserver(DataSetObserver observer) {

            }

            @Override
            public void unregisterDataSetObserver(DataSetObserver observer) {

            }

            @Override
            public int getCount() {
                return serverBeanList.size();
            }

            @Override
            public Object getItem(int position) {
                return serverBeanList.get(position);
            }

            @Override
            public long getItemId(int position) {
                return position;
            }

            @Override
            public boolean hasStableIds() {
                return false;
            }

            @Override
            public View getView(int position, View convertView, ViewGroup parent) {
                if (convertView == null) {
                    convertView = LayoutInflater.from(ListActivity.this).inflate(R.layout.spinner_item, null);
                    TextView textView = convertView.findViewById(R.id.text);
                    textView.setText("http://" + serverBeanList.get(position).getIp() + ":" + serverBeanList.get(position).getPort());
                }
                return convertView;
            }

            @Override
            public int getItemViewType(int position) {
                return 1;
            }

            @Override
            public int getViewTypeCount() {
                return 1;
            }

            @Override
            public boolean isEmpty() {
                return false;
            }
        });

        serverList.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                Log.e("position", position + "");
                setIp(serverBeanList, position);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                Log.e("position", "nonono");
            }
        });

        clearServer.setOnClickListener(v -> {
            serverBeanList.clear();
            SharedPreferences sp = getSharedPreferences(SETTING, Context.MODE_PRIVATE);
            SharedPreferences.Editor editor = sp.edit();
            editor.putString(SETTING_SERVER, "");
            editor.apply();

            toastInner("记录已清除");
        });

        list_show_type.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                switch (checkedId) {
                    case R.id.type_list:
                        list_show_type_num = 0;
                        break;
                    case R.id.type_scroll:
                        list_show_type_num = 1;
                        break;
                }
            }
        });
    }

    private void StapText(int s) {
        List<TextView> textViews = new ArrayList<>();
        textViews.add(text1);
        textViews.add(text2);
        textViews.add(text3);
        textViews.add(text4);
        textViews.add(text5);
        for (int i = 0; i < textViews.size(); i++) {
            textViews.get(i).setVisibility(View.GONE);
        }
        textViews.get(s).setVisibility(View.VISIBLE);
    }

    private void setIp(List<ServerBean> list, int index) {
        ServerBean serverBean = list.get(index);
        inputIp.setText(serverBean.getIp());
        inputPort.setText(serverBean.getPort());

        config.serverIp = serverBean.getIp();
        config.serverPort = Integer.parseInt(serverBean.getPort());

        mServerIp = "http://" + serverBean.getIp() + ":" + serverBean.getPort();

        Base.setServerAddr(useHttps, mServerIp);

        list.remove(serverBean);
        list.add(0, serverBean);

        SharedPreferences sp = getSharedPreferences(SETTING, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sp.edit();
        editor.putString(SETTING_SERVER, String.valueOf(JSON.toJSON(list)));
  /*      editor.putString(SETTING_SERVER, mServerIp);
        editor.putBoolean(SETTING_SERVER_USE_HTTPS, false);*/
        editor.apply();

        if (clientLifeManager == null) {
            clientLifeManager = new ClientLifeManager(this);
        }
        if (imSocketChannel == null) {
            imSocketChannel = new ImSocketChannel(socketChannelObserver, ListActivity.this);
        }
        if (getAppliList == null) {
            getAppliList = new GetAppliList(new GetAppliList.Callback() {
                @Override
                public void onSuccess(List<AppListItem> list) {
                    try {
                        Thread.sleep(2000);
                        setMessage(1, list);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }

                @Override
                public void onPageInfoChange(PageInfo pageInfo) {
                    mPage = pageInfo.getPageNum();
                    ListActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            if (pageInfo.hasPreviousPage()) {
                                lastPage.setImageResource(R.mipmap.lastpage);
                                lastPage.setEnabled(true);
                            } else {
                                lastPage.setImageResource(R.mipmap.lastpagefalse);
                                lastPage.setEnabled(false);
                            }

                            if (pageInfo.hasNextPage()) {
                                nextPage.setImageResource(R.mipmap.nextpage);
                                nextPage.setEnabled(true);
                            } else {

                                nextPage.setImageResource(R.mipmap.nextpagefalse);
                                nextPage.setEnabled(false);
                            }
                        }
                    });
                }

                @Override
                public void onFail(String s) {
                    Log.e("GetapplistFail", s);
                    /*toastInner(s);
                    getAppliList = null;
                    showSetupIP();*/
                }
            });
            getAppliList.getAppliList();
        }

        //imSocketChannel.connect();
        if (getRunMode == null) {
            getRunMode = new GetRunMode(new GetRunMode.Callback() {
                @Override
                public void onSuccess(GetRunModeBean getRunModeBean) {
                    try {
                        Thread.sleep(2000);
                        setMessage(2, getRunModeBean);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                }

                @Override
                public void onFail(String s) {
                    Log.e("getRunModeFaile", s);
                    /*toastInner(s);
                    showSetupIP();
                    getRunMode = null;*/
                }
            });
            getRunMode.dorequest(Util.getLocalMacAddress(ListActivity.this));
        }
    }

    private void StopApp(Boolean close) {
        if (clientLifeManager != null) {
            clientLifeManager.ClientOffline();
        }
        if (imSocketChannel != null) {
            if (imSocketChannel.isConnected()) {
                imSocketChannel.close();
            }
        }
        stoprunmode = close;
        if (close) {
            System.exit(0);
            Process.killProcess(Process.myPid());
        }
    }

    private void showSetupIP() {
        ListActivity.this.runOnUiThread(() -> {
            setIp.setVisibility(View.VISIBLE);
            appListAdapter = null;
            rec.setAdapter(null);
        });
    }

    private void closeSetupIP() {
        if (inputIp.getText().toString().isEmpty()) {
            toastInner("IP不能为空");
            return;
        }
        hideSoftInputFromWindow(this);
        setIp.setVisibility(View.GONE);

        ServerBean serverBean = new ServerBean();
        serverBean.setIp(inputIp.getText().toString());
        serverBean.setPort(inputPort.getText().toString());
        serverBean.setUse_https(false);

        boolean f = true;
        int index = 0;
        for (int i = 0; i < serverBeanList.size(); i++) {
            if (serverBeanList.get(i).getIp().equals(serverBean.getIp())) {
                if (serverBeanList.get(i).getPort().equals(serverBean.getPort())) {
                    f = false;
                    index = i;
                }
            }
        }
        if (f) {
            serverBeanList.add(serverBean);
            setIp(serverBeanList, serverBeanList.size() - 1);
        } else {
            setIp(serverBeanList, index);
        }
    }

    private void getRunMode() {
        if (stoprunmode) {
            return;
        }
        getRunMode.dorequest(Util.getLocalMacAddress(ListActivity.this));
    }

    private void toastInner(final String msg) {
        runOnUiThread(() -> Toast.makeText(ListActivity.this, msg, Toast.LENGTH_SHORT).show());
    }

    private void setMessage(int what, Object obj) {
        Message message = Message.obtain();
        message.what = what;
        message.obj = obj;
        //message.obj = ToJavaBean.toJavaBean(value,obj);
        handler.sendMessage(message);
    }

    private void setMessage(int what, Object obj, long time) {
        Message message = Message.obtain();
        message.what = what;
        message.obj = obj;
        //message.obj = ToJavaBean.toJavaBean(value,obj);
        handler.sendMessageDelayed(message, time);
    }

    @Override
    protected void onStart() {
        super.onStart();
        if (clientLifeManager != null) {
            clientLifeManager.ClientOnline();
        }
        if (imSocketChannel != null) {
            if (!imSocketChannel.isConnected()) {
                imSocketChannel.connect();
            }
            imSocketChannel.sendKeepAlive();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.d(TAG, "onPause");
        if (config != null) {
            Config.saveToCache(this, config);
        }
        if (clientLifeManager != null) {
            clientLifeManager.ClientOffline();
        }
        /*if (imSocketChannel != null) {
            if (imSocketChannel.isConnected()) {
                imSocketChannel.close();
            }
        }*/
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.d(TAG, "onStop");
    }

    @Override
    protected void onRestart() {
        super.onRestart();
        Log.d(TAG, "onRestart");
        if (clientLifeManager != null) {
            clientLifeManager.ClientOnline();
        }
        if (imSocketChannel != null) {
            if (!imSocketChannel.isConnected()) {
                imSocketChannel.connect();
            }
            imSocketChannel.sendKeepAlive();
        }
        Init();
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "onResume");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        StopApp(false);
    }

    /**
     * 隐藏输入面板
     *
     * @param activity
     * @return true 成功隐藏面板，false 没有隐藏面板或者没有面板可以隐藏
     */
    public static boolean hideSoftInputFromWindow(Activity activity) {
        if (activity != null) {
            InputMethodManager imm = (InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE);
            if (imm != null) {
                return imm.hideSoftInputFromWindow(activity.getWindow().getDecorView().getWindowToken(), 0);
            }
        }
        return false;
    }

    private SocketChannelObserver socketChannelObserver = new SocketChannelObserver() {
        @Override
        public void onOpen() {
            Log.e(TAG, "onOpen");
        }

        @Override
        public void onClose() {
            Log.e(TAG, "onClose");
        }

        @Override
        public void onError(String s) {
            Log.e(TAG, "onError:" + s);
        }

        @Override
        public void onMessage(byte[] bytes) {
            Log.e(TAG, "onMessagebyt:" + bytes.toString());
        }

        @Override
        public void onMessage(String s) {
            Log.e(TAG, "onMessagestr:" + s);
            try {
                JSONObject jsonObject = new JSONObject(s);
                switch (jsonObject.optString("type")) {
                    case ImSocketChannel.IM_MESSAGE_TYPE_KEEPALIVE: {
                        break;
                    }
                    case ImSocketChannel.IM_MESSAGE_TYPE_START: {
                        GoMainActivity(ListActivity.this,jsonObject.optString("appliId"));
                        break;
                    }
                    case ImSocketChannel.IM_MESSAGE_TYPE_STOP: {
                        Message message=new Message();
                        message.what=4;
                        BaseApplication.getInstance().getmHandler().sendMessage(message);
                        break;
                    }
                    case ImSocketChannel.IM_MESSAGE_TYPE_CONNECT_SUCCESS: {
                        toastInner("连接成功");
                        break;
                    }
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    };

    public void GoMainActivity(Context context, String appid) {
        Activity activity = (Activity) context;
        Intent intent = new Intent(activity, MainActivity.class);
        if (appid != null) {
            Log.e("GoMainActivity", appid);
            intent.putExtra("appid", appid);
        }else {
            Log.e("GoMainActivity", "justGo");
        }
        Intent extraIntent = new Intent("android.intent.action.MAIN");
        //Intent extraIntent = new Intent();
        extraIntent.addCategory("android.intent.category.LAUNCHER");
        extraIntent.setFlags(FLAG_ACTIVITY_SINGLE_TOP);
        intent.putExtra("intent", extraIntent);
        activity.startActivity(intent);
        //activity.finish();
    }

    private void getMessage(Message msg) {
        if (msg.what == 1) {
            List<AppListItem> locallist = (List<AppListItem>) msg.obj;

            switch (list_show_type_num) {
                case 0: {
                    animateBanner.setVisibility(View.GONE);
                    rec.setVisibility(View.VISIBLE);
                    list_background.setBackground(getResources().getDrawable(R.mipmap.border));
                    if (rec.getAdapter() == null) {
                        appListAdapter = new AppListAdapter(ListActivity.this, locallist);
                        rec.setAdapter(appListAdapter);
                    } else {
                        if (!locallist.equals(applist)) {
                            applist = locallist;
                            appListAdapter = new AppListAdapter(ListActivity.this, applist);
                            rec.setAdapter(appListAdapter);
                        }
                    }
                }
                break;
                case 1: {
                    animateBanner.setVisibility(View.VISIBLE);
                    rec.setVisibility(View.GONE);
                    list_background.setBackground(getResources().getDrawable(R.mipmap.homebackground));
                    animateBanner.setAutoPlay(false);
                    animateBanner.setClickSwitchable(true);
                    if (animateBanner.getItems() == null || animateBanner.getItems().isEmpty()) {
                        Log.e("ani", "animateBanner.getItems()==null");
                        animateBanner.setImagesToBanner(locallist);
                    } else {
                        Log.e("ani", "animateBannerNotnnull");
                        if (!locallist.equals(applist)) {
                            applist = locallist;
                            animateBanner.setImagesToBanner(applist);
                        }
                    }
                }
                break;
            }
            Log.e("getmessage", "getapplist");
            getAppliList.getAppliList();
        } else if (msg.what == 2) {
            GetRunModeBean getRunModeBean = (GetRunModeBean) msg.obj;
            Log.e("gerrunmode", getRunModeBean.toString());
            if (getRunModeBean.getCode() == 1000) {
                if (getRunModeBean.getResult().getRunMode().equals("1")) {
                    selfOnline.setVisibility(View.GONE);
                    SelfOnlineText.setVisibility(View.VISIBLE);
                    /* GoMainActivity(ListActivity.this, getRunModeBean.getResult().getPrimaryClientId());
                     getRunMode = null;*/
                } else {
                    selfOnline.setVisibility(View.VISIBLE);
                    SelfOnlineText.setVisibility(View.GONE);
                }
                getRunMode.dorequest(Util.getLocalMacAddress(ListActivity.this));
            } else {
                toastInner(getRunModeBean.getMessage());
            }
        } else if (msg.what == 3) {
            ListActivity.this.onPause();
            ListActivity.this.onStop();
        }
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent keyEvent) {
        Log.e("listkeyevent", keyEvent.getAction() + "");
        return super.dispatchKeyEvent(keyEvent);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        Log.e("listkeyCode", keyCode + "");
        return super.onKeyDown(keyCode, event);
    }
}