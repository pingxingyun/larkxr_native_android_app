package com.pxy.cloudlarkxrpico.UI;

import android.view.View;

import androidx.viewpager.widget.ViewPager;

public class NonPageTransformer implements ViewPager.PageTransformer {
    @Override
    public void transformPage(View page, float position)
    {
        page.setScaleX(0.999f);//hack
    }

    public static final ViewPager.PageTransformer INSTANCE = new NonPageTransformer();
}
