package com.pxy.larkxr_app_nolo.util;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;

import com.pxy.larkxr_app_nolo.R;

import java.io.IOException;
import java.io.InputStream;

public class Utils {
    //初始化纹理的方法
    public static int initTexture(GLSurfaceView gsv,int RepeatIndex)//textureId
    {
        int[] textures = new int[1];  //用于记录生成的纹理id
        GLES30.glGenTextures
                (
                        1,          //产生的纹理id的数量
                        textures,   //纹理id的数组
                        0           //偏移量
                );
        int textureId=textures[0];
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, textureId);
        //设置MIN采样方式
        GLES30.glTexParameterf(GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MIN_FILTER,GLES30.GL_NEAREST);
        //设置MAG采样方式
        GLES30.glTexParameterf(GLES30.GL_TEXTURE_2D,GLES30.GL_TEXTURE_MAG_FILTER,GLES30.GL_LINEAR);

        if(RepeatIndex==0)//如果索引值等于0
        {
            GLES30.glTexParameterf(GLES30.GL_TEXTURE_2D, //S轴为重复拉伸方式
                    GLES30.GL_TEXTURE_WRAP_S,GLES30.GL_REPEAT);
            GLES30.glTexParameterf(GLES30.GL_TEXTURE_2D, //T轴为重复拉伸方式
                    GLES30.GL_TEXTURE_WRAP_T,GLES30.GL_REPEAT);
        }
        else if(RepeatIndex==1)//如果索引值等于1
        {
            GLES30.glTexParameterf(GLES30.GL_TEXTURE_2D, //S轴为截取拉伸方式
                    GLES30.GL_TEXTURE_WRAP_S,GLES30.GL_CLAMP_TO_EDGE);
            GLES30.glTexParameterf(GLES30.GL_TEXTURE_2D, //T轴为截取拉伸方式
                    GLES30.GL_TEXTURE_WRAP_T,GLES30.GL_CLAMP_TO_EDGE);
        }else if(RepeatIndex==2)//如果索引值等于2
        {
            GLES30.glTexParameterf(GLES30.GL_TEXTURE_2D, //S轴为镜像重复拉伸方式
                    GLES30.GL_TEXTURE_WRAP_S,GLES30.GL_MIRRORED_REPEAT);
            GLES30.glTexParameterf(GLES30.GL_TEXTURE_2D, //T轴为镜像重复拉伸方式
                    GLES30.GL_TEXTURE_WRAP_T,GLES30.GL_MIRRORED_REPEAT);
        }

        //通过输入流加载图片===============begin===================
        @SuppressLint("ResourceType")
        InputStream is = gsv.getResources().openRawResource(R.drawable.robot);
        Bitmap bitmapTmp;
        try
        {
            bitmapTmp = BitmapFactory.decodeStream(is);
        }
        finally
        {
            try
            {
                is.close();
            }
            catch(IOException e)
            {
                e.printStackTrace();
            }
        }
        //通过输入流加载图片===============end=====================

        //实际加载纹理进显存
        GLUtils.texImage2D
                (
                        GLES30.GL_TEXTURE_2D,   //纹理类型
                        0, 					  //纹理的层次，0表示基本图像层，可以理解为直接贴图
                        bitmapTmp, 			  //纹理图像
                        0					  //纹理边框尺寸
                );
        bitmapTmp.recycle(); 		  //纹理加载成功后释放图片
        return textureId;
    }
}
