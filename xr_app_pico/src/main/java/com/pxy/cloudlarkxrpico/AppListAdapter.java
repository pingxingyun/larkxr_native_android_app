package com.pxy.cloudlarkxrpico;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.pxy.larkcore.request.AppListItem;
import com.pxy.larkcore.request.Base;
import com.pxy.larkcore.request.EnterAppliInfo;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.List;

import okhttp3.HttpUrl;

public class AppListAdapter extends RecyclerView.Adapter<AppListAdapter.ViewHolder> {
    private Context context;
    private List<AppListItem> appListItems;
    public AppListAdapter(Context context, List<AppListItem> appListItems){
        this.context=context;
        this.appListItems=appListItems;
    }
    @Override
    public ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i) {
        View view = LayoutInflater.from(viewGroup.getContext()).inflate(R.layout.applist_item,viewGroup,false);
        ViewHolder holder =new ViewHolder(view);
        return holder;
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder viewHolder, int i) {
        AppListItem data=appListItems.get(i);
        viewHolder.appname.setText(data.getAppliName());

        viewHolder.item.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent=new Intent(context, MainActivity.class);
                //intent.putExtra(EnterAppliInfo.Config.name,config);

                Intent extraIntent = new Intent("android.intent.action.MAIN");
                extraIntent.addCategory("android.intent.category.LAUNCHER");
                //extraIntent.setComponent(new ComponentName("com.DefaultCompany.UnityTestAndroid", "com.example.bootcomplete.MainActivity"));
                intent.putExtra("intent", extraIntent);
                context.startActivity(intent);
//                new EnterAppliInfo(new EnterAppliInfo.Callback() {
//                    @Override
//                    public void onSuccess(EnterAppliInfo.Config config) {
//
//                    }
//
//                    @Override
//                    public void onFail(String s) {
//                        toastInner(s);
//                    }
//                }).enterApp(data);
            }
        });
    }

    @Override
    public int getItemCount() {
        return appListItems.size();
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    static class ViewHolder extends RecyclerView.ViewHolder{
        TextView appname;
        LinearLayout item;
        public ViewHolder (View view)
        {
            super(view);
            appname=view.findViewById(R.id.appname);
            item=view.findViewById(R.id.item);
        }
    }

    private EnterAppliInfo.Config getparam(String result){
        try {
            JSONObject jsonObject = null;
            jsonObject = new JSONObject(result);

            String appServer = jsonObject.getString("serverIp");
            //int appPort = jsonObject.optInt("renderServerPort");
            int appPort = 10002;
            String appliId = jsonObject.getString("appliId");
            String taskId = jsonObject.getString("taskId");
            int initWinSize = jsonObject.optInt("initWinSize");
            String preferPubOutIp = jsonObject.optString("preferPubOutIp");
            //int wsProxy = Integer.parseInt(jsonObject.optString("wsProxy"));
            String roomCode = jsonObject.optString("roomCode", "");
            String bgColor = jsonObject.optString("bgColor", "000");
            boolean useGamepad = jsonObject.optInt("useGamepad", 0) == 1;
            String touchScreenMode = jsonObject.optString("touchOperateMode", "mouse");
            EnterAppliInfo.Config params = new EnterAppliInfo.Config();
            params.appServer = appServer;
            params.appPort = appPort;
            params.taskId = taskId;
            params.preferPubOutIp = preferPubOutIp;
            //params.noOperationTimeout = noOperationTimeout;
            //params.wsProxy = wsProxy;
            params.wsProxy = 1;
            params.roomCode = "null".equals(roomCode) ? "" : roomCode;
            // add "#" to begin.
            params.bgColor = bgColor.isEmpty() ? "" : "#" + bgColor;;
            // 设置服务器地址
            HttpUrl serverUrl = Base.getServerUrl().getUrl();
            params.webServerIp = serverUrl.host();
            params.webServerPort = serverUrl.port();
            params.useSecurityProtocol = Base.getServerUrl().useSecurityProtocol();
            params.useGamepad = useGamepad;
            params.appilId = appliId;
            params.touchOperateMode = touchScreenMode;

            return params;
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return null;
    }

    private void toastInner(final String msg) {
        Activity activity= (Activity) context;
        activity.runOnUiThread(() -> Toast.makeText(context, msg, Toast.LENGTH_SHORT).show());
    }
}
