package com.pxy.cloudlarkxroculus.Activity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.widget.ImageView;

import androidx.appcompat.app.AppCompatActivity;

import com.bumptech.glide.Glide;
import com.pxy.cloudlarkxroculus.R;
import com.pxy.cloudlarkxroculus.UI.Fastclick;

public class StartActivity extends Activity {
    private ImageView logo;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_start);

        logo=findViewById(R.id.logo);
        Glide.with(this).load(R.mipmap.logo).into(logo);
        logo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!Fastclick.isFastClick()) {
                    startListActivity();
                }
            }
        });
        Animation animation1 = new AlphaAnimation(0.1f,1.0f);
        animation1.setDuration(2000);

        Animation animation2 = new AlphaAnimation(1.0f,0f);
        animation2.setDuration(2000);
        animation2.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                startListActivity();
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }
        });

        animation1.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                logo.startAnimation(animation2);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }
        });
        logo.startAnimation(animation1);

    }

    private void startListActivity(){
            logo.setVisibility(View.GONE);
            startActivity(new Intent(StartActivity.this, ListActivity.class));
            finish();
    }
}