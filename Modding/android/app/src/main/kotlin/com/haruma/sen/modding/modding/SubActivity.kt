package com.haruma.sen.environment

import android.app.Activity
import android.content.ClipData
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.widget.Toast

class SubActivity : Activity() {

    public override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        try {
            val resource = mutableListOf<Uri>()
            when (intent.action) {
                Intent.ACTION_SEND -> {
                    intent.getParcelableExtra<Uri>(Intent.EXTRA_STREAM)?.let { resource.add(it) }
                }
                Intent.ACTION_SEND_MULTIPLE -> {
                    intent.getParcelableArrayListExtra<Uri>(Intent.EXTRA_STREAM)?.let { resource.addAll(it) }
                }
            }
            forwardResource(resource)
        } catch (e: Exception) {
            showException(e)
        } finally {
            finish()
        }
    }

    private fun forwardResource(resource: List<Uri>) {
        val intent = Intent(this, MainActivity::class.java).apply {
            action = "com.haruma.sen.environment.FORWARD"
            putParcelableArrayListExtra("resources", ArrayList(resource))
        }
        startActivity(intent)
    }

    private fun showException(exception: Exception) {
        Toast.makeText(this, exception.message, Toast.LENGTH_LONG).show()
    }
}
