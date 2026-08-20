package com.game.voicechanger

import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.Service
import android.content.Context
import android.content.Intent
import android.graphics.PixelFormat
import android.os.Build
import android.os.IBinder
import android.view.Gravity
import android.view.LayoutInflater
import android.view.View
import android.view.WindowManager
import android.widget.Button
import androidx.core.app.NotificationCompat

class VoiceService : Service() {

    private lateinit var windowManager: WindowManager
    private var overlayView: View? = null
    private var isVoiceActive = false

    external fun startVoiceEngine()
    external fun stopVoiceEngine()

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }

    override fun onBind(intent: Intent?): IBinder? = null

    override fun onCreate() {
        super.onCreate()
        startForegroundServiceNotification()
        showFloatingControl()
    }

    private fun showFloatingControl() {
        windowManager = getSystemService(Context.WINDOW_SERVICE) as WindowManager

        val params = WindowManager.LayoutParams(
            WindowManager.LayoutParams.WRAP_CONTENT,
            WindowManager.LayoutParams.WRAP_CONTENT,
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
                WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY
            else
                WindowManager.LayoutParams.TYPE_PHONE,
            WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
            PixelFormat.TRANSLUCENT
        ).apply {
            gravity = Gravity.CENTER_VERTICAL or Gravity.START
            x = 20
            y = 0
        }

        overlayView = LayoutInflater.from(this).inflate(R.layout.layout_floating_btn, null)
        val btnToggle = overlayView?.findViewById<Button>(R.id.btnToggle)

        btnToggle?.setOnClickListener {
            if (!isVoiceActive) {
                startVoiceEngine()
                btnToggle.text = "Girl Voice: ON"
            } else {
                stopVoiceEngine()
                btnToggle.text = "Girl Voice: OFF"
            }
            isVoiceActive = !isVoiceActive
        }

        windowManager.addView(overlayView, params)
    }

    private fun startForegroundServiceNotification() {
        val channelId = "VoiceChannel"
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(channelId, "Voice Service", NotificationManager.IMPORTANCE_LOW)
            val manager = getSystemService(NotificationManager::class.java)
            manager.createNotificationChannel(channel)
        }

        val notification = NotificationCompat.Builder(this, channelId)
            .setContentTitle("Voice Changer Active")
            .setContentText("Microphone is routed for Free Fire")
            .setSmallIcon(android.R.drawable.ic_btn_speak_now)
            .build()

        startForeground(1, notification)
    }

    override fun onDestroy() {
        super.onDestroy()
        stopVoiceEngine()
        if (overlayView != null) windowManager.removeView(overlayView)
    }
}

