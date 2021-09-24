package com.pxy.cloudlarkxrpico;

import static android.content.Context.TELEPHONY_SERVICE;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Build;
import android.os.Environment;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.UUID;

public class UniqueIDUtils {

    private static final String TAG = "UniqueIDUtils";
    private static String uniqueID;
    private static String uniqueKey = "unique_id";
    private static String uniqueIDDirPath = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOCUMENTS).getAbsolutePath();
    private static String uniqueIDFile = "unique.txt";


    public static String getUniqueID(Context context) {
        //三步读取：内存中，存储的SP表中，外部存储文件中
        if (!TextUtils.isEmpty(uniqueID)) {
            Log.e(TAG, "getUniqueID: 内存中获取" + uniqueID);
            return uniqueID;
        }
        uniqueID = PreferenceManager.getDefaultSharedPreferences(context).getString(uniqueKey, "");
        if (!TextUtils.isEmpty(uniqueID)) {
            Log.e(TAG, "getUniqueID: SP中获取" + uniqueID);
            return uniqueID;
        }
        readUniqueFile(context);
        if (!TextUtils.isEmpty(uniqueID)) {
            Log.e(TAG, "getUniqueID: 外部存储中获取" + uniqueID);
            return uniqueID;
        }
        //两步创建：硬件获取；自行生成与存储
        getDeviceID(context);
        getAndroidID(context);
        getSNID();
        createUniqueID(context);
        PreferenceManager.getDefaultSharedPreferences(context).edit().putString(uniqueKey, uniqueID);
        return uniqueID;
    }

    @SuppressLint("MissingPermission")
    private static void getDeviceID(Context context) {
        if (!TextUtils.isEmpty(uniqueID)) {
            return;
        }
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.O_MR1) {
            return;
        }
        String deviceId = null;
        try {
            deviceId = ((TelephonyManager) context.getSystemService(TELEPHONY_SERVICE)).getDeviceId();
            if (TextUtils.isEmpty(deviceId)) {
                return;
            }
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }
        uniqueID = deviceId;
        Log.e(TAG, "getUniqueID: DeviceId获取成功" + uniqueID);
    }

    private static void getAndroidID(Context context) {
        if (!TextUtils.isEmpty(uniqueID)) {
            return;
        }
        String androidID = null;
        try {
            androidID = Settings.Secure.getString(context.getContentResolver(), Settings.Secure.ANDROID_ID);
            if (TextUtils.isEmpty(androidID) || "9774d56d682e549c".equals(androidID)) {
                return;
            }
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }
        uniqueID = androidID;
        Log.e(TAG, "getUniqueID: AndroidID获取成功" + uniqueID);
    }

    private static void getSNID() {
        if (!TextUtils.isEmpty(uniqueID)) {
            return;
        }
        String snID = Build.SERIAL;
        if (TextUtils.isEmpty(snID)) {
            return;
        }
        uniqueID = snID;
        Log.e(TAG, "getUniqueID: SNID获取成功" + uniqueID);
    }


    private static void createUniqueID(Context context) {
        if (!TextUtils.isEmpty(uniqueID)) {
            return;
        }
        uniqueID = UUID.randomUUID().toString();
        Log.e(TAG, "getUniqueID: UUID生成成功" + uniqueID);
        File filesDir = new File(uniqueIDDirPath + File.separator + context.getApplicationContext().getPackageName());
        if (!filesDir.exists()) {
            filesDir.mkdir();
        }
        File file = new File(filesDir, uniqueIDFile);
        if (!file.exists()) {
            try {
                file.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        FileOutputStream outputStream = null;
        try {
            outputStream = new FileOutputStream(file);
            outputStream.write(uniqueID.getBytes());
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (outputStream != null) {
                try {
                    outputStream.flush();
                    outputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private static void readUniqueFile(Context context) {
        File filesDir = new File(uniqueIDDirPath + File.separator + context.getApplicationContext().getPackageName());
        File file = new File(filesDir, uniqueIDFile);
        if (file.exists()) {
            FileInputStream inputStream = null;
            try {
                inputStream = new FileInputStream(file);
                byte[] bytes = new byte[(int) file.length()];
                inputStream.read(bytes);
                uniqueID = new String(bytes);
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                if (inputStream != null) {
                    try {
                        inputStream.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

    public static void clearUniqueFile(Context context) {
        File filesDir = new File(uniqueIDDirPath + File.separator + context.getApplicationContext().getPackageName());
        deleteFile(filesDir);
    }

    private static void deleteFile(File file) {
        if (file.isDirectory()) {
            for (File listFile : file.listFiles()) {
                deleteFile(listFile);
            }
        } else {
            file.delete();
        }
    }
}

