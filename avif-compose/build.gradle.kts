plugins {
    kotlin("multiplatform")
    id("com.android.library")
    id("org.jetbrains.compose")
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
            withAndroidTarget()
            group("skia") {
                withJvm()
                withIosX64()
                withIosArm64()
                withIosSimulatorArm64()
            }
        }
    }
    sourceSets {
        commonMain {
            dependencies {
                api(projects.avif)
                api(compose.ui)
            }
        }
    }
    jvmToolchain(11)
}

android {
    compileSdk = (findProperty("android.compileSdk") as String).toInt()
    namespace = "com.seiko.avif.compose"
    sourceSets["main"].manifest.srcFile("src/androidMain/AndroidManifest.xml")
    defaultConfig {
        minSdk = (findProperty("android.minSdk") as String).toInt()
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
}
