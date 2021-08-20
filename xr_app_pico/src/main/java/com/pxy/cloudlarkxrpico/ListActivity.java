package com.pxy.cloudlarkxrpico;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.widget.SwitchCompat;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.pxy.cloudlarkxrkit.Config;
import com.pxy.larkcore.ClientLifeManager;
import com.pxy.larkcore.SocketChannelObserver;
import com.pxy.larkcore.request.AppListItem;
import com.pxy.larkcore.request.Base;
import com.pxy.larkcore.request.GetAppliList;
import com.pxy.larkcore.request.ImServerSocket;
import com.pxy.larkcore.request.PageInfo;

import java.text.DecimalFormat;
import java.util.List;

public class ListActivity extends Activity {
    private static final String TAG = "MainActivity";

    private static final String SETTING = "pxy_setting";
    private static final String SETTING_SERVER = "serverAddress";
    private static final String SETTING_SERVER_USE_HTTPS = "useHttps";
    private static final String SETTING_LIST_3D= "list3D";
    private ClientLifeManager clientLifeManager;
    private Config config;

    private String mServerIp = "";
    private boolean useHttps;

    private Button settingIP,settingTab,confirmip,closeip,closeSetTab;
    private RecyclerView rec;

    private LinearLayout setIp,advancedList,setTab;
    private EditText inputIp,inputPort;

    private AppListAdapter appListAdapter;

    private SwitchCompat advancedSetting,list3D,vibrator,UseH265,reportFecFailed,UseFovRending,Use10Bit;
    private RadioGroup QuickConfigLevel;
    private RadioButton QuickConfigLevel_Manual,QuickConfigLevel_Auto,QuickConfigLevel_Fast,QuickConfigLevel_Normal,QuickConfigLevel_Extreme;

    private SeekBar resolutionScaleBar,coderateBar;
    private TextView resolutionScale,coderate;

    private RadioGroup StreamType;
    private RadioButton larkStreamType_UDP,larkStreamType_TCP,larkStreamType_THROTTLED_UDP;

    private ImServerSocket imServerSocket;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Init();
        setContentView(R.layout.activity_list);
        FindViewById();
        initview();
    }

    private void Init(){
        SharedPreferences sp = getSharedPreferences(SETTING, Context.MODE_PRIVATE);
        //boolean list3D=sp.getBoolean(SETTING_LIST_3D,false);
        if (false){
            finish();
            startActivity(new Intent(ListActivity.this, MainActivity.class));
        }

        mServerIp = sp.getString(SETTING_SERVER, "");
        useHttps = sp.getBoolean(SETTING_SERVER_USE_HTTPS, false);

        clientLifeManager=new ClientLifeManager(this);
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

        StreamType=findViewById(R.id.StreamType);
        larkStreamType_UDP=findViewById(R.id.larkStreamType_UDP);
        larkStreamType_TCP=findViewById(R.id.larkStreamType_TCP);
        larkStreamType_THROTTLED_UDP=findViewById(R.id.larkStreamType_THROTTLED_UDP);

        UseH265=findViewById(R.id.UseH265);
        reportFecFailed=findViewById(R.id.reportFecFailed);
        UseFovRending=findViewById(R.id.UseFovRending);
        Use10Bit=findViewById(R.id.Use10Bit);
    }
    private void readconfig() {
        config=Config.readFromCache(this);
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
                resolutionScale.setText(config.resulutionScale+"");
                coderate.setText(config.biteRate+"");

                UseH265.setChecked(config.useH265);
                reportFecFailed.setChecked(config.reportFecFailed);
                UseFovRending.setChecked(config.useFovRendering);
                Use10Bit.setChecked(config.use10BitEncoder);
            }
        });
    }
    private void initview() {
        Log.d(TAG, "cached server address use https " + useHttps + " serverIp " + mServerIp);

        if (mServerIp == null || mServerIp.isEmpty()) {
            Log.d(TAG, "unset serverAddress");
            showSetupIP();
        } else {
            String outhttp=mServerIp.substring(7);
            Log.e("outhttp",outhttp);
            inputIp.setText(outhttp.split(":")[0]);
            inputPort.setText(outhttp.split(":")[1]);
            Base.setServerAddr(useHttps, mServerIp);
            dorequest();
        }



        settingIP.setOnClickListener(v -> showSetupIP());
        closeip.setOnClickListener(v -> setIp.setVisibility(View.GONE));

        settingTab.setOnClickListener(v -> {
            readconfig();
            setTab.setVisibility(View.VISIBLE);});

        confirmip.setOnClickListener(v -> closeSetupIP());

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
                        DecimalFormat df=new DecimalFormat("0.00");
                        resolutionScale.setText(df.format((float)progress/100));
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
    }


    private void showSetupIP() {
        setIp.setVisibility(View.VISIBLE);
    }

    private void closeSetupIP(){
        if (inputIp.getText().toString().isEmpty()){
            toastInner("IP不能为空");
            return;
        }

        mServerIp="http://"+inputIp.getText().toString()+":"+inputPort.getText().toString();
        SharedPreferences sp = getSharedPreferences(SETTING, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sp.edit();
        editor.putString(SETTING_SERVER, mServerIp);
        editor.putBoolean(SETTING_SERVER_USE_HTTPS, false);
        editor.apply();
        Base.setServerAddr(false, mServerIp);
        setIp.setVisibility(View.GONE);
        dorequest();
    }

    private void dorequest() {
        if (mServerIp == null || mServerIp.isEmpty()) {
            Log.d(TAG, "unset serverAddress");
            showSetupIP();
            return;
        }

        imServerSocket=new ImServerSocket(socketChannelObserver);
        imServerSocket.connect();

        new GetAppliList(new GetAppliList.Callback() {
            @Override
            public void onSuccess(List<AppListItem> list) {
                setMessage(1,list);
            }
            @Override
            public void onPageInfoChange(PageInfo pageInfo) {
            }
            @Override
            public void onFail(String s) {
                toastInner(s);
            }
        }).getAppliList();
    }

    private void toastInner(final String msg) {
        runOnUiThread(() -> Toast.makeText(ListActivity.this, msg, Toast.LENGTH_SHORT).show());
    }

    public Handler handler = new Handler() {
        public void handleMessage(Message msg) {
            getMessage(msg);
        }
    };

    private void setMessage(int what,Object obj){
        Message message = Message.obtain();
        message.what=what;
        message.obj=obj;
        //message.obj = ToJavaBean.toJavaBean(value,obj);
        handler.sendMessage(message);
    }

    private void getMessage(Message msg){
        if (msg.what==1){
            List<AppListItem> list= (List<AppListItem>) msg.obj;
            appListAdapter=new AppListAdapter(ListActivity.this,list);
            rec.setAdapter(appListAdapter);
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        clientLifeManager.ClientOnline();
    }

    @Override
    protected void onPause() {
        super.onPause();
        clientLifeManager.ClientOffline();
    }

    @Override
    protected void onResume() {
        super.onResume();
        clientLifeManager.ClientOnline();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        clientLifeManager.ClientOffline();
        imServerSocket.close();
    }

    private SocketChannelObserver socketChannelObserver=new SocketChannelObserver() {
        @Override
        public void onOpen() {

        }

        @Override
        public void onClose() {

        }

        @Override
        public void onError(String s) {

        }

        @Override
        public void onMessage(byte[] bytes) {

        }
    };
}