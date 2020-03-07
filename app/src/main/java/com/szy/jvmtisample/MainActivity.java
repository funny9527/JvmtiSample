package com.szy.jvmtisample;

import android.os.Bundle;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Method;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "test";
    private static final String AGENT_NAME = "libmyagent.so";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final String path = this.getApplicationInfo().nativeLibraryDir + File.separator + AGENT_NAME;

        new Thread(
                new Runnable() {
                    @Override
                    public void run() {
                        InputStream in = null;
                        OutputStream os = null;
                        try {
                            in = new FileInputStream(new File(path));
                            String target = MainActivity.this.getCacheDir() + File.separator + AGENT_NAME;
                            os = new FileOutputStream(target);

                            byte[] data = new byte[4096];
                            while (in.read(data) != -1) {
                                os.write(data);
                            }
                            Class cls = null;
                            try {
                                cls = Class.forName("dalvik.system.VMDebug");
                                Method method = cls.getMethod("attachAgent", String.class);
                                method.invoke(cls, target);
                            } catch (Exception e) {
                                Log.e(TAG, "", e);
                            }
                        } catch (Exception e) {
                            Log.d(TAG, "", e);
                        } finally {
                            try {
                                in.close();
                                os.close();
                            } catch (Exception e) {
                                Log.e(TAG, "", e);
                            }
                        }
                    }
                }
        ).start();

    }

}
