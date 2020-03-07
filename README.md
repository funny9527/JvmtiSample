# JvmtiSample
This is a jvmti sample for android
A jvmti agent is a native library who can attach to a process. 
First, build your agent to an so library, then attach your agent to target process as follows:
1. use command line
   push agent to your private folder, you can use ```run-as``` command.
   use attach command like:
   ```
   cmd activity attach-agent pid /data/data/packagename/libmyagent.so
   ```
2. attach in application process
   use class ```dalvik.system.VMDebug``` and ```attachAgent``` method. See sample code to find detail information.
   
