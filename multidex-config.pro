#为 MultiDex 应用构建每个 DEX 文件时，构建工具会执行复杂的决策制定以确定主要 DEX 文件中需要的类，以便您的应用能够成功启动。
#如果主要 DEX 文件中未提供启动期间需要的任何类，则您的应用会崩溃并出现 java.lang.NoClassDefFoundError 错误
#对于直接从您的应用代码访问的代码，不应发生这种情况，因为构建工具可以识别这些代码路径。
#但是，当代码路径的可见性较低时（例如，当您使用的库具有复杂的依赖项时），可能会发生这种情况。
# 例如，如果代码使用自检机制或从原生代码调用 Java 方法，那么可能不会将这些类识别为主要 DEX 文件中的必需类

# issue： main dex不生效
#方案一
#.在min sdk >= 21 时，Android Gradle Plugin，会忽视一切指定分包规则的参数，比如multiDexKeepProguard
#高版本解决方案
# additionalParameters = [
  #'--multi-dex',//多dex分包
  #'--set-max-idx-number=60000',//每个包内方法数上限
  #'--main-dex-list=' + projectDir + '/main-dex-rule', //打包到主classes.dex的文件列表
  #'--minimal-main-dex'
  #]
# https://github.com/Tencent/tinker/issues/1084

#方案二
# minSdkVersion 19

#main dex
-keep class com.kd.** { *; }

-keep class com.sankuai.** { *; }
-keep class com.taobao.trip.** { *; }
