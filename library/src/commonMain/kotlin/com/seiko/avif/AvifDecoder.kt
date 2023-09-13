package com.seiko.avif

expect class AvifDecoder {
    companion object {
        fun create(bytes: ByteArray): AvifDecoder
    }

    fun nextImage(): Boolean

    fun getImage(): AvifImage

    fun getImageCount(): Int

    fun getImageDurationMs(): Int

    fun close()
}
