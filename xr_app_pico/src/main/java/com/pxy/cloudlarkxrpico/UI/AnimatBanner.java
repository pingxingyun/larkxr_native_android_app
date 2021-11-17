package com.pxy.cloudlarkxrpico.UI;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.viewpager.widget.PagerAdapter;
import androidx.viewpager.widget.ViewPager;

import com.bumptech.glide.Glide;
import com.pxy.cloudlarkxrpico.Activity.ListActivity;
import com.pxy.cloudlarkxrpico.R;
import com.pxy.larkcore.request.AppListItem;
import com.pxy.larkcore.request.Base;

import java.util.ArrayList;
import java.util.List;

public class AnimatBanner extends ViewGroup {
    private List<AppListItem> items = new ArrayList<>();

    public List<AppListItem> getItems() {
        return items;
    }

    private ViewPager bannerPager;
    private int deplayedTime = 3000;
    private boolean isAutoPlay = true;
    private boolean clickSwitchable = true;

    private int imgWidth = dip2px(300);//图片宽度  默认为76dp
    private int imgHeight = dip2px(170);//图片宽度  默认为76dp
    private int pageMargin = dip2px(20);//图片之间的间距  默认为15dp
    private float scalingRatio = 0.5f;

    /**
     * 根据手机的分辨率从 dp 的单位 转成为 px(像素)
     */
    public int dip2px(float dpValue) {
        final float scale = getContext().getResources().getDisplayMetrics().density;
        return (int) (dpValue * scale + 0.5f);
    }

    public AnimatBanner(Context context) {
        this(context, null);
    }

    public AnimatBanner(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public AnimatBanner(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        setClipChildren(false);
    }


    /**
     * 设置是否自动轮播  在setImagesToBanner前调用
     *
     * @param isAutoPlay
     */
    public void setAutoPlay(boolean isAutoPlay) {
        this.isAutoPlay = isAutoPlay;
    }

    /**
     * 设置居中显示的图片的大小 单位为dp  默认为76dp
     *
     * @param imgWidth
     * @param imgHeight
     */
    public void setImgSize(int imgWidth, int imgHeight) {
        this.imgWidth = dip2px(imgWidth);
        this.imgHeight = dip2px(imgHeight);
    }

    /**
     * 设置图片之间的间距  单位为dp  默认为15dp
     *
     * @param pageMargin
     */
    public void setPageMargin(int pageMargin) {
        this.pageMargin = dip2px(pageMargin);
    }

    /**
     * 设置是否可以点击切换  默认为可以切换
     * 此方法可在任意时间调用
     */
    public void setClickSwitchable(boolean clickSwitchable) {
        this.clickSwitchable = clickSwitchable;
    }

    /**
     * 设置缩放比例
     *
     * @param scalingRatio
     */
    public void setScalingRatio(float scalingRatio) {
        this.scalingRatio = scalingRatio;
    }

    /**
     * 开始轮播
     */
    public void startPlay() {
        if (handler != null && isAutoPlay && items != null && items.size() > 0) {
            handler.removeMessages(1);
            handler.sendEmptyMessageDelayed(1, deplayedTime);
        }
    }

    /**
     * 停止轮播
     */
    public void onPause() {
        if (handler != null) {
            handler.removeMessages(1);
        }
    }

    /**
     * 销毁handler  防止内存泄露
     */
    public void onDestroy() {
        if (handler != null) {
            handler.removeMessages(1);
            handler = null;
        }
    }

    /**
     * 传入图片
     *
     * @param urls
     */
    public void setImagesToBanner(final List urls) {

        removeAllViews();

        if (urls == null || urls.isEmpty()) {
            return;
        }

        this.items = urls;

        bannerPager = new ViewPager(getContext());

        bannerPager.setPageMargin(pageMargin);//图片之间的间距
        int fixed_num = 3;
        bannerPager.setOffscreenPageLimit((urls.size() >= fixed_num ? fixed_num : urls.size()) + fixed_num);//解决滑动很多个条目式因预加载的数量不够而显示空白的问题
        bannerPager.setPageTransformer(true, (ViewPager.PageTransformer) new ScaleInTransformer(scalingRatio));//设置viewpager滑动样式   scalingRatio为缩放比例  根据蓝湖上标注计算得出
        bannerPager.setAdapter(new BannerPagerAdapter());
        bannerPager.setCurrentItem(urls.size() * 100);//无限轮播
        bannerPager.setClipChildren(false);

        bannerPager.addOnPageChangeListener(new ViewPager.OnPageChangeListener() {
            @Override
            public void onPageScrolled(int i, float v, int i1) {

            }

            @Override
            public void onPageSelected(int i) {
                startPlay();
            }

            @Override
            public void onPageScrollStateChanged(int i) {

            }
        });
        addView(bannerPager);
        startPlay();
    }

    /**
     * 计算父布局宽度  使得刚好展示出左右两侧图片
     *
     * @param widthMeasureSpec
     * @param heightMeasureSpec
     */
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        measureChildren(widthMeasureSpec, heightMeasureSpec);
        int width = (int) (bannerPager.getMeasuredWidth() + (imgWidth * scalingRatio) * 2 + pageMargin * 2);
        setMeasuredDimension(width, imgHeight);
    }

    /**
     * @param changed
     * @param l
     * @param t
     * @param r
     * @param b
     */
    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        View view = getChildAt(0);//拿到唯一的一个viewpager

        int left = (int) ((imgWidth * scalingRatio) + pageMargin);
        int top = 0;
        int right = (int) (((imgWidth * scalingRatio) + pageMargin) + view.getMeasuredWidth());
        int bottom = imgHeight;

        view.layout(left, top, right, bottom);//将viewpager绘制到父布局中
    }

    private int clickPosition = -1;

    //当点击的位置不在viewpager上时  事件才会返回到该布局上  再将其分发到viewpager上 嘤嘤嘤
    @SuppressLint("ClickableViewAccessibility")
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                Log.e("ACTION_DOWN", "ACTION_DOWN");
                if (event.getX() > bannerPager.getX()) {//判断按下的位置是在viewpager左边还是右边  1：右  0：左
                    clickPosition = 1;
                } else {
                    clickPosition = 0;
                }
                // Log.e("test", clickPosition + "-"+event.getX()+"-`"+bannerPager.getX());
                break;
            case MotionEvent.ACTION_MOVE://如果移动那么点击事件不生效  clickable为false
                Log.e("ACTION_MOVE", "ACTION_MOVE");
                break;
            case MotionEvent.ACTION_UP://没有移动  点击事件生效  根据位置切换viewpager
                Log.e("ACTION_UP", clickSwitchable + "-" + clickPosition);
                if (clickSwitchable && clickPosition != -1) {
                    Log.e("ACTION_UP", "true");
                    if (clickPosition == 0) {//点击viewpager左侧
                        bannerPager.setCurrentItem(bannerPager.getCurrentItem() - 1);
                    } else {//点击viewpager右侧
                        bannerPager.setCurrentItem(bannerPager.getCurrentItem() + 1);
                    }
                }
                break;
        }

        return bannerPager.onTouchEvent(event);//事件最终全部分发到viewpager上
    }

    class BannerPagerAdapter extends PagerAdapter {
        @Override
        public Object instantiateItem(ViewGroup container, final int position) {
            LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(imgWidth,
                    imgHeight);//设置图片大小也相当于设置viewpager的大小

            layoutParams.gravity= Gravity.CENTER_HORIZONTAL;

            container.setLayoutParams(layoutParams);//container为布局容器  需要手动设置container大小并将其居中

            AppListItem data = items.get(position % items.size());
            View view = LayoutInflater.from(getContext()).inflate(R.layout.applist_item, null);
            TextView appname = view.findViewById(R.id.appname);
            ImageView pic = view.findViewById(R.id.pic);
            Glide.with(getContext())
                    .load(Base.getServerUrl().getUrl() + data.getPicUrl())
                    //.apply(circleOptions)
                    .error(R.mipmap.cover_11)
                    .into(pic);
            appname.setText(data.getAppliName());
            pic.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    ListActivity listActivity= (ListActivity) getContext();
                    listActivity.GoMainActivity(listActivity,data.getAppliId());
                }
            });
            container.addView(view);
            return view;
            /*RelativeLayout.LayoutParams layoutParams = new RelativeLayout.LayoutParams(imgWidth,
                    imgHeight);//设置图片大小也相当于设置viewpager的大小

            layoutParams.addRule(RelativeLayout.CENTER_IN_PARENT);//viewapger居中显示

            container.setLayoutParams(layoutParams);//container为布局容器  需要手动设置container大小并将其居中

            ImageView imageView = new ImageView(getContext());//创建需要加载的图片
            imageView.setLayoutParams(layoutParams);//设置布局方式   可有可无
            //imageView.setScaleType(ImageView.ScaleType.CENTER_CROP);//设置图片缩放模式

//            RequestOptions circleOptions = new RequestOptions()
//                    .circleCrop();//设置圆角
            imageView.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    if ( urls.get(position % urls.size()).getToactivity()!=null){
                        getContext().startActivity(new Intent(getContext(),urls.get(position % urls.size()).getToactivity()));
                    }
                }
            });

            Glide.with(getContext())
                    .load(urls.get(position % urls.size()).getPic())
                    //.apply(circleOptions)
                    .into(imageView);

            container.addView(imageView);

            return imageView;*/
        }


        @Override
        public void destroyItem(ViewGroup container, int position, Object object) {
            container.removeView((View) object);
        }

        @Override
        public int getCount() {
            return Integer.MAX_VALUE;
        }

        @Override
        public boolean isViewFromObject(View view, Object o) {
            return view == o;
        }

    }

    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == 1) {
                bannerPager.setCurrentItem(bannerPager.getCurrentItem() + 1);
            }
        }
    };
}
