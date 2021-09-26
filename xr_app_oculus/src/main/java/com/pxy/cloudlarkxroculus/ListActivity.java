package com.pxy.cloudlarkxroculus;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.DataSetObserver;
import android.os.Build;
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
import android.widget.SimpleAdapter;
import android.widget.Spinner;
import android.widget.SpinnerAdapter;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.widget.SwitchCompat;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.alibaba.fastjson.JSON;
import com.bumptech.glide.Glide;
import com.pxy.cloudlarkxrkit.Config;
import com.pxy.cloudlarkxroculus.MainActivity;
import com.pxy.cloudlarkxroculus.R;
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
import com.pxy.larkcore.request.ScheduleTaskManager;

import org.json.JSONException;
import org.json.JSONObject;

import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

public class ListActivity extends Activity {
    private static final String TAG = "pvr_activity_list";
    private static final String SETTING = "pxy_setting";
    private static final String SETTING_LIST_3D= "list3D";
    //网络设置
    private static final String SETTING_SERVER = "serverAddress";
    private static final String SETTING_SERVER_USE_HTTPS = "useHttps";
    private List<ServerBean> serverBeanList=new ArrayList<>();
    private Spinner serverList;
    //生命周期管理
    private ClientLifeManager clientLifeManager;
    private Config config=null;
    //IP
    private String mServerIp = "";
    private boolean useHttps;

    //设置按钮和列表
    private Button settingIP,settingTab,confirmip,closeip,closeSetTab;
    private RecyclerView rec;
    //设置面板
    private LinearLayout setIp,advancedList,setTab,selfOnline;
    private EditText inputIp,inputPort;
    //列表adapter
    private AppListAdapter appListAdapter;
    //列表组件
    private SwitchCompat advancedSetting,list3D,vibrator,UseH265,reportFecFailed,UseFovRending,Use10Bit;
    private RadioGroup QuickConfigLevel;
    private RadioButton QuickConfigLevel_Manual,QuickConfigLevel_Auto,QuickConfigLevel_Fast,QuickConfigLevel_Normal,QuickConfigLevel_Extreme;
    private SeekBar resolutionScaleBar,coderateBar;
    private TextView resolutionScale,coderate,SelfOnlineText;
    private RadioGroup StreamType;
    private RadioButton larkStreamType_UDP,larkStreamType_TCP,larkStreamType_THROTTLED_UDP;
    //socket链接
    private ImSocketChannel imSocketChannel;
    //getrunmode接口开关
    private Boolean stoprunmode=false;
    //翻页请求
    private GetAppliList getAppliList;
    List<AppListItem> applist;
    private int mPage=1;
    private ImageView lastPage,nextPage;

    //关闭app按钮
    private ImageView closeApp;

    // 定时任务循环
    private ScheduleTaskManager mScheduleTaskManager;
    //getrunmode
    private GetRunMode getRunMode;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.e("over---","over---");
        Log.e("deviceId ",getDeviceId().isEmpty()?"deviceId":getDeviceId());
        Log.e("macAdress",Util.getLocalMacAddress(ListActivity.this));

        BaseApplication.getInstance().setmHandler(handler);
        setContentView(R.layout.activity_list);
        FindViewById();
        Init();
        initview();


    }
    /**
     * 获取设备唯一ID
     * @return
     */
    public static String getDeviceId() {
        String m_szDevIDShort = "35" + (Build.BOARD.length() % 10) + (Build.BRAND.length() % 10) + (Build.CPU_ABI.length() % 10) + (Build.DEVICE.length() % 10) + (Build.MANUFACTURER.length() % 10) + (Build.MODEL.length() % 10) + (Build.PRODUCT.length() % 10);
        String serial = null;
        try {
            serial = Build.class.getField("SERIAL").get(null).toString();
            return new UUID(m_szDevIDShort.hashCode(), serial.hashCode()).toString();
        } catch (Exception exception) {
            serial = "serial";
        }
        return new UUID(m_szDevIDShort.hashCode(), serial.hashCode()).toString();
    }

    public Handler handler = new Handler() {
        public void handleMessage(Message msg) {
            getMessage(msg);
        }
    };

    private void Init(){
        SharedPreferences sp = getSharedPreferences(SETTING, Context.MODE_PRIVATE);
        boolean list3Dbool=sp.getBoolean(SETTING_LIST_3D,false);
        list3D.setChecked(list3Dbool);
        if (list3Dbool){
         GoMainActivity(ListActivity.this,null);
        }
        if (!sp.getString(SETTING_SERVER, "").isEmpty()){
            serverBeanList=JSON.parseArray(sp.getString(SETTING_SERVER, ""),ServerBean.class);
            mServerIp="http://"+serverBeanList.get(0).getIp()+":"+serverBeanList.get(0).getPort();
            useHttps=serverBeanList.get(0).getUse_https();
        }

        /*mServerIp = sp.getString(SETTING_SERVER, "");
        useHttps = sp.getBoolean(SETTING_SERVER_USE_HTTPS, false);*/
        CloudlarkManager.init(this,CloudlarkManager.APP_TYPE_VR);

        readconfig();
    }

    private void FindViewById() {
        settingIP=findViewById(R.id.settingIP);
        settingTab=findViewById(R.id.settingTab);
        rec=findViewById(R.id.rec);
/*        int screenWidth = getWindowManager().getDefaultDisplay().getWidth(); // 屏幕宽（像素，如：480px）
        int screenHeight = getWindowManager().getDefaultDisplay().getHeight(); // 屏幕高（像素，如：800p）
        int spancount=screenWidth>screenHeight?4:2;*/
        GridLayoutManager gridLayoutManager=new GridLayoutManager(this,4);
        rec.setLayoutManager(gridLayoutManager);

        selfOnline=findViewById(R.id.SelfOnline);    

        setTab=findViewById(R.id.setTab);
        setIp =findViewById(R.id.setIP);
        inputIp=findViewById(R.id.inputIp);
        inputPort=findViewById(R.id.inputPort);
        confirmip=findViewById(R.id.confirmip);
        closeip=findViewById(R.id.closeip);
        closeSetTab=findViewById(R.id.closeSetTab);

        advancedSetting=findViewById(R.id.advancedSetting);
        list3D=findViewById(R.id.list3D);
        advancedList=findViewById(R.id.advancedList);
        vibrator=findViewById(R.id.vibrator);

        QuickConfigLevel=findViewById(R.id.QuickConfigLevel);
        QuickConfigLevel_Manual=findViewById(R.id.QuickConfigLevel_Manual);
        QuickConfigLevel_Auto=findViewById(R.id.QuickConfigLevel_Auto);
        QuickConfigLevel_Fast=findViewById(R.id.QuickConfigLevel_Fast);
        QuickConfigLevel_Normal=findViewById(R.id.QuickConfigLevel_Normal);
        QuickConfigLevel_Extreme=findViewById(R.id.QuickConfigLevel_Extreme);

        resolutionScaleBar=findViewById(R.id.resolutionScaleBar);
        resolutionScale=findViewById(R.id.resolutionScale);
        coderateBar=findViewById(R.id.coderateBar);
        coderate=findViewById(R.id.coderate);

        SelfOnlineText=findViewById(R.id.SelfOnlineText);

        StreamType=findViewById(R.id.StreamType);
        larkStreamType_UDP=findViewById(R.id.larkStreamType_UDP);
        larkStreamType_TCP=findViewById(R.id.larkStreamType_TCP);
        larkStreamType_THROTTLED_UDP=findViewById(R.id.larkStreamType_THROTTLED_UDP);

        UseH265=findViewById(R.id.UseH265);
        reportFecFailed=findViewById(R.id.reportFecFailed);
        UseFovRending=findViewById(R.id.UseFovRending);
        Use10Bit=findViewById(R.id.Use10Bit);

        lastPage=findViewById(R.id.lastPage);
        nextPage=findViewById(R.id.nextPage);

        closeApp=findViewById(R.id.closeApp);

        serverList=findViewById(R.id.serverList);
    }

    private void readconfig() {
        config=Config.readFromCache(this);

        if (mServerIp == null || mServerIp.isEmpty()) {
            Log.d(TAG, "unset serverAddress");
            showSetupIP();
        } else {
            setIp(serverBeanList.get(0));
            dorequest();
        }

        ListActivity.this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                switch (config.quickConfigLevel){
                    case Config.QuickConfigLevel_Manual:{
                        QuickConfigLevel.check(R.id.QuickConfigLevel_Manual);
                        break;
                    }
                    case Config.QuickConfigLevel_Auto:{
                        QuickConfigLevel.check(R.id.QuickConfigLevel_Auto);
                        break;
                    }
                    case Config.QuickConfigLevel_Normal:{
                        QuickConfigLevel.check(R.id.QuickConfigLevel_Normal);
                        break;
                    }
                    case Config.QuickConfigLevel_Fast:{
                        QuickConfigLevel.check(R.id.QuickConfigLevel_Fast);
                        break;
                    }
                    case Config.QuickConfigLevel_Extreme:{
                        QuickConfigLevel.check(R.id.QuickConfigLevel_Extreme);
                        break;
                    }
                }
                vibrator.setChecked(config.vibrator);

                Log.e("resulutionScale", config.resulutionScale+"");

                int resulutionScaleint= (int) (config.resulutionScale*100);
                resolutionScale.setText(resulutionScaleint+"");
                coderate.setText(config.biteRate+"");

                UseH265.setChecked(config.useH265);
                reportFecFailed.setChecked(config.reportFecFailed);
                UseFovRending.setChecked(config.useFovRendering);
                Use10Bit.setChecked(config.use10BitEncoder);

                resolutionScaleBar.setProgress(resulutionScaleint);
                coderateBar.setProgress(config.biteRate/10000);
            }
        });
    }

    private void initview() {
        Log.d(TAG, "cached server address use https " + useHttps + " serverIp " + mServerIp);

        settingIP.setOnClickListener(v -> showSetupIP());
        closeip.setOnClickListener(v -> setIp.setVisibility(View.GONE));

        settingTab.setOnClickListener(v -> {
            readconfig();
            setTab.setVisibility(View.VISIBLE);});

        confirmip.setOnClickListener(v ->closeSetupIP());

        advancedSetting.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked){
                advancedList.setVisibility(View.VISIBLE);
            }else {
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
            if (list3D.isChecked()){
                GoMainActivity(ListActivity.this,null);
            }
        });

        QuickConfigLevel.setOnCheckedChangeListener((group, checkedId) -> {
            switch (checkedId){
                case R.id.QuickConfigLevel_Manual:{
                    config.quickConfigLevel=Config.QuickConfigLevel_Manual;
                    break;
                }
                case R.id.QuickConfigLevel_Auto:{
                    config.quickConfigLevel=Config.QuickConfigLevel_Auto;
                    break;
                }
                case R.id.QuickConfigLevel_Fast:{
                    config.quickConfigLevel=Config.QuickConfigLevel_Fast;
                    break;
                }
                case R.id.QuickConfigLevel_Normal:{
                    config.quickConfigLevel=Config.QuickConfigLevel_Normal;
                    break;
                }
                case R.id.QuickConfigLevel_Extreme:{
                    config.quickConfigLevel=Config.QuickConfigLevel_Extreme;
                    break;
                }
            }
        });

        vibrator.setOnCheckedChangeListener((buttonView, isChecked) -> {
            config.vibrator=isChecked;
        });

        resolutionScaleBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                ListActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        /*DecimalFormat df=new DecimalFormat("0.00");
                        resolutionScale.setText(df.format((float)progress/100));*/
                        resolutionScale.setText(progress+"");
                        config.resulutionScale=(float)progress/100;
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
                        coderate.setText(progress*10000+"");
                        config.biteRate=progress*10000;
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
                switch (checkedId){
                    case R.id.larkStreamType_UDP:{
                        config.streamType=Config.larkStreamType_UDP;
                        break;
                    }
                    case R.id.larkStreamType_TCP:{
                        config.streamType=Config.larkStreamType_TCP;
                        break;
                    }
                    case R.id.larkStreamType_THROTTLED_UDP:{
                        config.streamType=Config.larkStreamType_THROTTLED_UDP;
                        break;
                    }
                }
            }
        });

        UseH265.setOnCheckedChangeListener((buttonView, isChecked) -> {
            config.useH265=isChecked;
        });

        reportFecFailed.setOnCheckedChangeListener((buttonView, isChecked) -> {
            config.reportFecFailed=isChecked;
        });

        UseFovRending.setOnCheckedChangeListener((buttonView, isChecked) -> {
            config.useFovRendering=isChecked;
        });

        Use10Bit.setOnCheckedChangeListener((buttonView, isChecked) -> {
            config.use10BitEncoder=isChecked;
        });

        lastPage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getAppliList.setPage(--mPage);
                getAppliList.getAppliList();
            }
        });

        nextPage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getAppliList.setPage(++mPage);
                getAppliList.getAppliList();
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
                if (convertView==null){
                    convertView=LayoutInflater.from(ListActivity.this).inflate(R.layout.spinner_item,null);
                    TextView textView=convertView.findViewById(R.id.text);
                    textView.setText("http://"+serverBeanList.get(position).getIp()+":"+serverBeanList.get(position).getPort());
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
                if (convertView==null){
                    convertView=LayoutInflater.from(ListActivity.this).inflate(R.layout.spinner_item,null);
                    TextView textView=convertView.findViewById(R.id.text);
                    textView.setText("http://"+serverBeanList.get(position).getIp()+":"+serverBeanList.get(position).getPort());
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
                Log.e("position",position+"");
                setIp(serverBeanList.get(position));
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                Log.e("position","nonono");
            }
        });
    }

    private void setIp(ServerBean serverBean){
        inputIp.setText(serverBean.getIp());
        inputPort.setText(serverBean.getPort());
        mServerIp="http://"+serverBean.getIp()+":"+serverBean.getPort();
        Base.setServerAddr(useHttps, mServerIp);
        config.serverIp=serverBean.getIp();
        config.serverPort= Integer.parseInt(serverBean.getPort());
    }

    private void StopApp(Boolean close){
        if(clientLifeManager!=null) {
            clientLifeManager.ClientOffline();
        }
        if (imSocketChannel!=null) {
            if (imSocketChannel.isConnected()) {
                imSocketChannel.close();
            }
        }
        stoprunmode =close;
        if (close) {
            System.exit(0);
            Process.killProcess(Process.myPid());
        }
    }

    private void showSetupIP() {
        setIp.setVisibility(View.VISIBLE);
    }

    private void closeSetupIP(){
        if (inputIp.getText().toString().isEmpty()){
            toastInner("IP不能为空");
            return;
        }
        setIp.setVisibility(View.GONE);
        ServerBean serverBean=new ServerBean();
        serverBean.setIp(inputIp.getText().toString());
        serverBean.setPort(inputPort.getText().toString());
        serverBean.setUse_https(false);
        setIp(serverBean);
        dorequest();
        for (ServerBean s:serverBeanList){
            if (s.getIp().equals(serverBean.getIp())){
                if (s.getPort().equals(serverBean.getPort())){
                    return;
                }
            }
        }
        serverBeanList.add(serverBean);

        SharedPreferences sp = getSharedPreferences(SETTING, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sp.edit();
        editor.putString(SETTING_SERVER, String.valueOf(JSON.toJSON(serverBeanList)));
  /*      editor.putString(SETTING_SERVER, mServerIp);
        editor.putBoolean(SETTING_SERVER_USE_HTTPS, false);*/
        editor.apply();
    }

    private void dorequest() {
        if (mServerIp == null || mServerIp.isEmpty()) {
            Log.d(TAG, "unset serverAddress");
            showSetupIP();
            return;
        }

        clientLifeManager=new ClientLifeManager(this);
        imSocketChannel=new ImSocketChannel(socketChannelObserver,ListActivity.this);

        if (getAppliList==null) {
            getAppliList = new GetAppliList(new GetAppliList.Callback() {
                @Override
                public void onSuccess(List<AppListItem> list) {
                    setMessage(1, list);
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
                    toastInner(s);
                }
            });
        }
        getAppliList.getAppliList();

        //imSocketChannel.connect();
        if (getRunMode==null) {
            getRunMode = new GetRunMode(new GetRunMode.Callback() {
                @Override
                public void onSuccess(GetRunModeBean getRunModeBean) {
                    setMessage(2, getRunModeBean, 2000);
                }
                @Override
                public void onFail(String s) {
                    Log.e("getRunModeFaile", s);
                }
            });
        }
        getRunMode.dorequest(Util.getLocalMacAddress(ListActivity.this));
    }

    private void getRunMode(){
        if (stoprunmode){
            return;
        }
        getRunMode.dorequest(Util.getLocalMacAddress(ListActivity.this));
    }

    private void toastInner(final String msg) {
        runOnUiThread(() -> Toast.makeText(ListActivity.this, msg, Toast.LENGTH_SHORT).show());
    }

    private void toJavaBeanS(String s){
        try {
            JSONObject jsonObject=new JSONObject(s);
            switch (jsonObject.optString("type")){
                case ImSocketChannel.IM_MESSAGE_TYPE_KEEPALIVE:{
                    break;
                }
                case ImSocketChannel.IM_MESSAGE_TYPE_START:{

                    break;
                }
                case ImSocketChannel.IM_MESSAGE_TYPE_STOP:{

                    break;
                }
                case ImSocketChannel.IM_MESSAGE_TYPE_CONNECT_SUCCESS:{
                    toastInner("连接成功");
                    break;
                }
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private void setMessage(int what,Object obj){
        Message message = Message.obtain();
        message.what=what;
        message.obj=obj;
        //message.obj = ToJavaBean.toJavaBean(value,obj);
        handler.sendMessage(message);
    }

    private void setMessage(int what,Object obj,long time){
        Message message = Message.obtain();
        message.what=what;
        message.obj=obj;
        //message.obj = ToJavaBean.toJavaBean(value,obj);
        handler.sendMessageDelayed(message,time);
    }

    @Override
    protected void onStart() {
        super.onStart();
        if(clientLifeManager!=null){
            clientLifeManager.ClientOnline();
        }
        if (imSocketChannel!=null) {
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
        if (config!=null) {
            Config.saveToCache(this, config);
        }
        if(clientLifeManager!=null) {
            clientLifeManager.ClientOffline();
        }
        if (imSocketChannel!=null) {
            if (imSocketChannel.isConnected()) {
                imSocketChannel.close();
            }
        }
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
        if(clientLifeManager!=null) {
            clientLifeManager.ClientOnline();
        }
        if (imSocketChannel!=null) {
            if (!imSocketChannel.isConnected()) {
                imSocketChannel.connect();
            }
            imSocketChannel.sendKeepAlive();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "onResume");
        Init();
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

    private SocketChannelObserver socketChannelObserver=new SocketChannelObserver() {
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
            Log.e(TAG, "onError:"+s);
        }

        @Override
        public void onMessage(byte[] bytes) {
            Log.e(TAG, "onMessagebyt:"+bytes.toString());
        }

        @Override
        public void onMessage(String s) {
            Log.e(TAG, "onMessagestr:"+s);
            toJavaBeanS(s);
        }
    };

    private void GoMainActivity(Context context,String appid){
        Activity activity= (Activity) context;
        Intent intent=new Intent(activity, MainActivity.class);
        if (appid!=null) {
            intent.putExtra("appid", appid);
        }
/*        Intent extraIntent = new Intent("android.intent.action.MAIN");
        //Intent extraIntent = new Intent();
        extraIntent.addCategory("android.intent.category.LAUNCHER");
        extraIntent.setFlags(FLAG_ACTIVITY_SINGLE_TOP);
        intent.putExtra("intent", extraIntent);*/
        activity.startActivity(intent);
        //activity.finish();
    }

    private void getMessage(Message msg){
        if (msg.what==1){
/*            List<AppListItem> list= (List<AppListItem>) msg.obj;
            appListAdapter=new AppListAdapter(ListActivity.this,list);
            rec.setAdapter(appListAdapter);*/
            List<AppListItem> locallist= (List<AppListItem>) msg.obj;
            if (!locallist.equals(applist)){
                applist=locallist;
                appListAdapter=new AppListAdapter(ListActivity.this,applist);
                rec.setAdapter(appListAdapter);
            }
            new Thread(()-> {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                getAppliList.getAppliList();
            }).start();
        }else if (msg.what==2){
            GetRunModeBean getRunModeBean= (GetRunModeBean) msg.obj;
            Log.e("gerrunmode", getRunModeBean.toString());
            if (getRunModeBean.getCode()==1000){
                if (getRunModeBean.getResult().getRunMode().equals("1")){
                    selfOnline.setVisibility(View.GONE);
                    SelfOnlineText.setVisibility(View.VISIBLE);
                    GoMainActivity(ListActivity.this, getRunModeBean.getResult().getPrimaryClientId());
                }else {
                    selfOnline.setVisibility(View.VISIBLE);
                    SelfOnlineText.setVisibility(View.GONE);
                }
                getRunMode();
            }else {
                toastInner(getRunModeBean.getMessage());
            };

        }else if (msg.what==3){
            ListActivity.this.onPause();
            ListActivity.this.onStop();
        }
    }

    public class AppListAdapter extends  RecyclerView.Adapter<AppListAdapter.ViewHolder> {
        private Context context;
        private List<AppListItem> appListItems;
        private AppListAdapter(Context context, List<AppListItem> appListItems){
            this.context=context;
            this.appListItems=appListItems;
        }
        @Override
        public AppListAdapter.ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i) {
            View view = LayoutInflater.from(viewGroup.getContext()).inflate(R.layout.applist_item,viewGroup,false);
            AppListAdapter.ViewHolder holder =new AppListAdapter.ViewHolder(view);
            return holder;
        }

        @Override
        public void onBindViewHolder(AppListAdapter.ViewHolder viewHolder, int i) {
            AppListItem data=appListItems.get(i);
            Log.e("viewholder", data.getAppliName());
            Log.e("viewholderpic", data.getPicUrl());
            viewHolder.appname.setText(data.getAppliName());
            viewHolder.appid.setText(data.getAppliId());
            viewHolder.item.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    GoMainActivity(context,data.getAppliId());
                }
            });
            Glide.with(context)
                    .load(Base.getServerUrl().getUrl()+data.getPicUrl())
                    .error(R.mipmap.cover_11)
                    .into(viewHolder.pic);
        }

        @Override
        public int getItemCount() {
            return appListItems.size();
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        private class ViewHolder extends RecyclerView.ViewHolder{
            TextView appname,appid;
            LinearLayout item;
            ImageView pic;
            public ViewHolder (View view)
            {
                super(view);
                appname=view.findViewById(R.id.appname);
                item=view.findViewById(R.id.item);
                appid=view.findViewById(R.id.appid);
                pic=view.findViewById(R.id.pic);
            }
        }

    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent keyEvent) {
        Log.e("listkeyevent", keyEvent.getAction()+"");
        return super.dispatchKeyEvent(keyEvent);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        Log.e("listkeyCode", keyCode+"");
        return super.onKeyDown(keyCode, event);
    }
}