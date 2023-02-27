package com.electrolytej.titan

import android.content.ContentProvider
import android.content.ContentValues
import android.database.Cursor
import android.net.Uri
import android.util.Log

class InitiatorContentProvider : ContentProvider() {
    override fun onCreate(): Boolean {
        Log.d("cjf", "InitiatorContentProvider#onCreate ${context?.javaClass?.canonicalName}")

        return true
    }

    override fun query(
        uri: Uri,
        projection: Array<out String>?,
        selection: String?,
        selectionArgs: Array<out String>?,
        sortOrder: String?
    ): Cursor? {
        throw  IllegalAccessError("不支持访问")
    }


    override fun getType(uri: Uri): String? {
        throw  IllegalAccessError("不支持访问")
    }

    override fun insert(uri: Uri, values: ContentValues?): Uri? {
        throw  IllegalAccessError("不支持访问")
    }


    override fun delete(uri: Uri, selection: String?, selectionArgs: Array<out String>?): Int {
        throw  IllegalAccessError("不支持访问")
    }


    override fun update(
        uri: Uri,
        values: ContentValues?,
        selection: String?,
        selectionArgs: Array<out String>?
    ): Int {
        throw  IllegalAccessError("不支持访问")
    }
}