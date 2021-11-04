package com.pxy.cloudlarkxrpico.Adapter;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.bumptech.glide.Glide;
import com.pxy.cloudlarkxrpico.R;
import com.pxy.larkcore.request.AppListItem;
import com.pxy.larkcore.request.Base;

import java.util.List;

public class AppListAdapter extends RecyclerView.Adapter<AppListAdapter.ViewHolder> {
    private Context context;
    private List<AppListItem> appListItems;

    public AppListAdapter(Context context, List<AppListItem> appListItems) {
        this.context = context;
        this.appListItems = appListItems;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i) {
        View view = LayoutInflater.from(viewGroup.getContext()).inflate(R.layout.applist_item, viewGroup, false);
        ViewHolder holder = new ViewHolder(view);
        return holder;
    }

    @Override
    public void onBindViewHolder(ViewHolder viewHolder, int i) {
        AppListItem data = appListItems.get(i);
        Log.e("viewholder", data.getAppliName());
        viewHolder.appname.setText(data.getAppliName());
        viewHolder.appid.setText(data.getAppliId());
        viewHolder.item.setOnClickListener(v -> {
            ListActivity activity= (ListActivity) context;
            activity.GoMainActivity(context, data.getAppliId());
        });

        Glide.with(context)
                .load(Base.getServerUrl().getUrl() + data.getPicUrl())
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

    public class ViewHolder extends RecyclerView.ViewHolder {
        TextView appname, appid;
        LinearLayout item;
        ImageView pic;

        public ViewHolder(View view) {
            super(view);
            appname = view.findViewById(R.id.appname);
            item = view.findViewById(R.id.item);
            appid = view.findViewById(R.id.appid);
            pic = view.findViewById(R.id.pic);
        }
    }
}
