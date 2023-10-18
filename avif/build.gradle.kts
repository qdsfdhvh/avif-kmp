import org.jetbrains.kotlin.gradle.plugin.mpp.KotlinNativeTarget

plugins {
    kotlin("multiplatform")
    id("com.android.library")
    id("com.vanniktech.maven.publish")
}

kotlin {
    androidTarget {
        publishAllLibraryVariants()
    }
    jvm()
    iosX64()
    iosArm64()
    iosSimulatorArm64()
    @Suppress("OPT_IN_USAGE")
    targetHierarchy.custom {
        common {
            group("jni") {
                withAndroidTarget()
                withJvm()
            }
            group("skia") {
                group("native") {
                    group("ios") {
                        withIosX64()
                        withIosArm64()
                        withIosSimulatorArm64()
                    }
                }
                withJvm()
            }
        }
    }
    sourceSets {
        all {
            languageSettings {
                optIn("kotlinx.cinterop.ExperimentalForeignApi")
            }
        }
        @Suppress("UNUSED_VARIABLE")
        val skiaMain by getting {
            dependencies {
                api("org.jetbrains.skiko:skiko:0.7.80")
            }
        }
    }
    targets.withType<KotlinNativeTarget> {
        val main by compilations.getting

        val arch = when (name) {
            "iosSimulatorArm64" -> "aarch64-sim"
            "iosX64" -> "aarch64"
            "iosArm64" -> "x86_64"
            else -> error("avif not support with target: $name")
        }

        val libraryPath = file("$rootDir/avif/darwin/build/ios/$arch")
        main.cinterops {
            create("avif") {
                defFile("src/nativeMain/cinterop/avif.def")

                includeDirs(file("darwin/libavif/include"))

                extraOpts("-libraryPath", "$libraryPath")

                // extraOpts(
                //     // "-Xsource-compiler-option", "-DONLY_C_LOCALE=1",
                //     // "-Xsource-compiler-option", "-std=c99",
                //     // "-Xsource-compiler-option", "-x",
                //     // "-Xsource-compiler-option", "c++",
                //     // "-Xsource-compiler-option", "-std=c++14",
                //     // "-Xcompile-source", "${projectDir}/src/nativeMain/cpp/avifImageNative.cpp",
                // )
            }
        }
        main.kotlinOptions {
            // https://youtrack.jetbrains.com/issue/KT-39396
            freeCompilerArgs += listOf(
                "-include-binary",
                "$libraryPath/libdav1d.a",
                "-include-binary",
                "$libraryPath/libavif.a",
            )
        }
    }
    jvmToolchain(11)
}

android {
    compileSdk = (findProperty("android.compileSdk") as String).toInt()
    namespace = "com.seiko.avif"
    sourceSets["main"].manifest.srcFile("src/androidMain/AndroidManifest.xml")
    defaultConfig {
        minSdk = (findProperty("android.minSdk") as String).toInt()
        ndk {
            abiFilters += listOf("x86", "x86_64", "armeabi-v7a", "arm64-v8a")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
    buildTypes {
        @Suppress("UNUSED_VARIABLE", "UnstableApiUsage")
        val release by getting {
            externalNativeBuild {
                cmake {
                    cFlags("-fvisibility=hidden")
                    cppFlags("-fvisibility=hidden")
                }
            }
        }
    }
    externalNativeBuild {
        cmake {
            path = file("src/androidMain/CMakeLists.txt")
        }
    }
    ndkVersion = "25.1.8937393"
}

val buildLibAvifDarwinKLib by tasks.creating(Exec::class) {
    group = "avif"

    inputs.files(projectDir.resolve("build-jvm-klib.sh"))

    workingDir = projectDir

    commandLine(
        buildList {
            add("bash")
            add("-l")
            add("build-jvm-klib.sh")
            findProperty("ARCH")?.let {
                add("-a")
                add(it)
            }
            findProperty("CMAKE-ARCH")?.let {
                add("-c")
                add(it)
            }
        }
    )
}
