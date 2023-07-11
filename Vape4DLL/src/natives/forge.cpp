#include "forge.h"

void SetupEventShit(PJVM_CTX ctx)
{
    jint forgeVersion = GetForgeVersion(app_ctx);

    if (forgeVersion > 13)
    {
        if (forgeVersion < 26)
        {
            forge_ctx->clsEvent = GetClassByName(ctx, "net/minecraftforge/fml/common/eventhandler/Event");
            forge_ctx->clsListenerList = GetClassByName(ctx, "net/minecraftforge/fml/common/eventhandler/ListenerList");
            forge_ctx->clsListenerListInstance =
                GetClassByName(ctx, "net/minecraftforge/fml/common/eventhandler/ListenerList$ListenerListInst");
            forge_ctx->clsEventPriority =
                GetClassByName(ctx, "net/minecraftforge/fml/common/eventhandler/EventPriority");

            forge_ctx->fidEventPriority = GetFieldByName(ctx, forge_ctx->clsEventPriority, "NORMAL",
                                                         "Lnet/minecraftforge/fml/common/eventhandler/EventPriority;");

            forge_ctx->midGetListenerList =
                GetMethodByName(ctx, forge_ctx->clsEvent, "getListenerList",
                                "()Lnet/minecraftforge/fml/common/eventhandler/ListenerList;");
            forge_ctx->midRegister = GetMethodByName(ctx, forge_ctx->clsListenerList, "register",
                                                     "(ILnet/minecraftforge/fml/common/eventhandler/EventPriority;Lnet/"
                                                     "minecraftforge/fml/common/eventhandler/IEventListener;)V");

            forge_ctx->fidLists =
                GetFieldByName(ctx, forge_ctx->clsListenerList, "lists",
                               "[Lnet/minecraftforge/fml/common/eventhandler/ListenerList$ListenerListInst;");
            forge_ctx->fidAllLists = GetFieldByName(ctx, forge_ctx->clsListenerList, "allLists",
                                                    "Lcom/google/common/collect/ImmutableList;");
        }
        else
        {
            forge_ctx->clsEvent = GetClassByName(ctx, "net/minecraftforge/eventbus/api/Event");
            forge_ctx->clsListenerList = GetClassByName(ctx, "net/minecraftforge/eventbus/ListenerList");
            forge_ctx->clsListenerListInstance =
                GetClassByName(ctx, "net/minecraftforge/eventbus/ListenerList$ListenerListInst");
            forge_ctx->clsEventPriority = GetClassByName(ctx, "net/minecraftforge/eventbus/api/EventPriority");

            forge_ctx->fidEventPriority = GetFieldByName(ctx, env, forge_ctx->clsEventPriority, "NORMAL",
                                                         "Lnet/minecraftforge/eventbus/api/EventPriority;");

            forge_ctx->midGetListenerList = GetMethodByName(ctx, env, forge_ctx->clsEvent, "getListenerList",
                                                            "()Lnet/minecraftforge/eventbus/ListenerList;");
            forge_ctx->midRegister = GetMethodByName(
                ctx, env, forge_ctx->clsListenerList, "register",
                "(ILnet/minecraftforge/eventbus/api/EventPriority;Lnet/minecraftforge/eventbus/api/IEventListener;)V");
        }
    }
    else
    {
        forge_ctx->clsEvent = GetClassByName(ctx, "cpw/mods/fml/common/eventhandler/Event");
        forge_ctx->clsListenerList = GetClassByName(app_ctx, env, "cpw/mods/fml/common/eventhandler/ListenerList");
        forge_ctx->clsListenerListInstance =
            GetClassByName(app_ctx, env, "net/minecraftforge/fml/common/eventhandler/ListenerList$ListenerListInst");
        forge_ctx->clsEventPriority = GetClassByName(app_ctx, env, "cpw/mods/fml/common/eventhandler/EventPriority");

        forge_ctx->fidEventPriority = GetFieldByName(app_ctx, env, forge_ctx->clsEventPriority, "NORMAL",
                                                     "Lcpw/mods/fml/common/eventhandler/EventPriority;");

        forge_ctx->midGetListenerList = GetMethodByName(app_ctx, env, forge_ctx->clsEvent, "getListenerList",
                                                        "()Lcpw/mods/fml/common/eventhandler/ListenerList;");
        forge_ctx->midRegister = GetMethodByName(
            app_ctx, env, forge_ctx->clsListenerList, "register",
            "(ILcpw/mods/fml/common/eventhandler/EventPriority;Lcpw/mods/fml/common/eventhandler/IEventListener;)V");

        forge_ctx->midUnregister = GetMethodByName(app_ctx, env, forge_ctx->clsListenerList, "unregisterAll",
                                                   "(ILcpw/mods/fml/common/eventhandler/IEventListener;)V");

        forge_ctx->fidLists = GetFieldByName(app_ctx, env, forge_ctx->clsListenerList, "lists",
                                             "[Lcpw/mods/fml/common/eventhandler/ListenerList;");
        forge_ctx->fidAllLists = GetFieldByName(app_ctx, env, forge_ctx->clsListenerList, "allLists",
                                                "Lcom/google/common/collect/ImmutableList;");
    }

    forge_ctx->midBuildCache = GetMethodByName(app_ctx, env, forge_ctx->clsListenerListInstance, "buildCache", "()V");
}

void DefineRegisterNative()
{
    JNIEnv *env;

    jclass clsClassLoader = env->FindClass("java/lang/ClassLoader");
    jmethodID midDefineClass = env->GetMethodID(clsClassLoader, "defineClass", "([BII)Ljava/lang/Class;");

    // TEMP...
    typedef struct ctx_t
    {
        jobject objClassLoader;
    };
    ctx_t *ctx;

    jsize classLength;
    jbyte *classBytes;

    jbyteArray array = env->NewByteArray(classLength);
    env->SetByteArrayRegion(array, 0, classLength, reinterpret_cast<jbyte *>(classBytes));
    jclass cls = (jclass)env->CallObjectMethod(ctx->objClassLoader, midDefineClass, array);
    jclass clsEventHandler = (jclass)env->NewGlobalRef(cls);
    env->DeleteLocalRef(cls);

    jbyte *cbuff = (jbyte *)malloc(classLength);
    memset(cbuff, 0, classLength);
    env->SetByteArrayRegion(array, 0, classLength, cbuff);
    env->DeleteLocalRef(array);

    jmethodID midEventHandlerInit = GET_CONTSTRUCTOR(clsEventHandler);

    auto reg = [clsEventHandler, midEventHandlerInit](JNIEnv *env, jclass cls_c, jclass cls, jint busId) {
        jclass clsEvent = (jclass)env->NewGlobalRef(cls);
        jobject objVapeListener = env->NewGlobalRef(env->NewObject(clsEventHandler, midEventHandlerInit));

        jmethodID midInit = GET_CONTSTRUCTOR(clsEvent);

        jobject objEvent = env->NewGlobalRef(env->NewObject(clsEvent, midInit));
        jobject objListenerList = env->CallObjectMethod(objEvent, forge_ctx->midGetListenerList);
        jobject objEventPriority = env->GetStaticObjectField(forge_ctx->clsEventPriority, forge_ctx->fidEventPriority);

        env->CallVoidMethod(objListenerList, forge_ctx->midRegister, busId, objEventPriority, objVapeListener);

        // TODO: add to map to be unregistered when Vape exits.
    };
}

void UnregisterEventSubscriptions()
{
}