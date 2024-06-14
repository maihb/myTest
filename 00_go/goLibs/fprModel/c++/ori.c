////
//

#include <sys/types.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <iconv.h>
#include <sched.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> //readlink

#include "../../../sdk/gals.h"
#include "../../../sdk/gamc.h"
#include "image.h"

gchar UTF8[512];
gchar g_exepath[512];
gint g_exepathlen = 0;

// #define INDIA

#define _T(en, cn) trantext(en, cn)
#ifndef INDIA
#define GALSDLLNAME "libgals0410.so"
#define GAMCDLLNAME "libgamc0410.so" //
#define SCANNERTYPE "S680"
#define CPRTEN "Copyright (C) Hongda Opto-Electron CO.,LTD."
#define CPRTCH "版权所有(C) 长春鸿达光电子与生物统计识别技术有限公司"
#define TITLEEN "S680 Fingerprint Scanner 2.4"
#define TITLECH "S680 指纹采集仪 2.4"
#define PNGFILE "S68016.png"
#else
#define GALSDLLNAME "libgals0410.so"
#define GAMCDLLNAME "libgamc0410.so" //
#define SCANNERTYPE "LS1000M"
#define TITLEEN "LS1000M Fingerprint Scanner"
#define TITLECH "LS1000M 指纹采集仪"
#define CPRTEN "IBIOS Scanner"
#define CPRTCH "IBIOS 采集仪"
#define PNGFILE "S68016.png"
#endif

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 640
gint g_disp_width = (DEFAULT_WIDTH / 2);
gint g_disp_height = (DEFAULT_HEIGHT / 2);
int g_PauseCount = 0;

GtkWindow *g_main_win = NULL;
GtkWidget *g_ImgFrame = NULL;

void *g_pdlGals = NULL;
void *g_pdlGamc = NULL;
void *g_phDisp = NULL;
gboolean g_bInit = 0;
gint g_PauseCont = 0;
gint g_nRollMode = 4;

guchar *g_pImgBuf = NULL;
guchar *g_pCaptureBuf = NULL;

//=============================================================================
LIVESCAN_Init_ LS_Init = NULL;
LIVESCAN_Close_ LS_Close = NULL;
LIVESCAN_GetChannelCount_ LS_GetChannelCount = NULL;
LIVESCAN_SetBright_ LS_SetBright = NULL;
LIVESCAN_SetContrast_ LS_SetContrast = NULL;
LIVESCAN_GetBright_ LS_GetBright = NULL;
LIVESCAN_GetContrast_ LS_GetContrast = NULL;
LIVESCAN_GetMaxImageSize_ LS_GetMaxImageSize = NULL;
LIVESCAN_GetCaptWindow_ LS_GetCaptWindow = NULL;
LIVESCAN_SetCaptWindow_ LS_SetCaptWindow = NULL;
LIVESCAN_Setup_ LS_Setup = NULL;
LIVESCAN_GetErrorInfo_ LS_GetErrorInfo = NULL;
LIVESCAN_BeginCapture_ LS_BeginCapture = NULL;
LIVESCAN_GetFPRawData_ LS_GetFPRawData = NULL;
LIVESCAN_EndCapture_ LS_EndCapture = NULL;
LIVESCAN_IsSupportCaptWindow_ LS_IsSupportCaptWindow = NULL;
LIVESCAN_IsSupportSetup_ LS_IsSupportSetup = NULL;
LIVESCAN_GetVersion_ LS_GetVersion = NULL;
LIVESCAN_GetDesc_ LS_GetDesc = NULL;
LIVESCAN_RawToBmp_ LS_RawToBmp = NULL;
LIVESCAN_BmpToRaw_ LS_BmpToRaw = NULL;

LIVESCAN_SetFrameRate_ LS_SetFrameRate = NULL;
LIVESCAN_GetFrameRate_ LS_GetFrameRate = NULL;

MOSAIC_Init_ MC_Init = NULL;
MOSAIC_Close_ MC_Close = NULL;
MOSAIC_IsSupportIdentifyFinger_ MC_IsSupportIdentifyFinger = NULL;
MOSAIC_IsSupportImageQuality_ MC_IsSupportImageQuality = NULL;
MOSAIC_IsSupportFingerQuality_ MC_IsSupportFingerQuality = NULL;
MOSAIC_IsSupportImageEnhance_ MC_IsSupportImageEnhance = NULL;
MOSAIC_IsSupportRollCap_ MC_IsSupportRollCap = NULL;
MOSAIC_SetRollMode_ MC_SetRollMode = NULL;
MOSAIC_Start_ MC_Start = NULL;
MOSAIC_DoMosaic_ MC_DoMosaic = NULL;
MOSAIC_Stop_ MC_Stop = NULL;
MOSAIC_ImageQuality_ MC_ImageQuality = NULL;
MOSAIC_FingerQuality_ MC_FingerQuality = NULL;
MOSAIC_ImageEnhance_ MC_ImageEnhance = NULL;
MOSAIC_IsFinger_ MC_IsFinger = NULL;
MOSAIC_GetErrorInfo_ MC_GetErrorInfo = NULL;
MOSAIC_GetVersion_ MC_GetVersion = NULL;
MOSAIC_GetDesc_ MC_GetDesc = NULL;

//=============================================================================
pthread_t g_tprcsid = 0;
pthread_t g_tdspid = 0;

int g_terminate_prcs = 0;
int g_terminate_dsp = 0;

sem_t g_sem;

gboolean load_dll();
void *process_thread(void *param);
void *display_thread(void *param);
//=============================================================================

int get_exe_path(char *Path, int Size)
{
    char *path_end;
    if (readlink("/proc/self/exe", Path, Size) <= 0)
        return 0;
    path_end = strrchr(Path, '/');
    if (path_end == NULL)
        return 0;
    ++path_end;
    *path_end = '\0';
    return (size_t)(path_end - Path);
}

//
int code_convert(char *from_charset, char *to_charset, char *inbuf, int inlen, char *outbuf, int outlen)
{
    iconv_t cd;
    int ret;

    if ((cd = iconv_open(to_charset, from_charset)) == 0)
    {
        return -1;
    }

    memset(outbuf, 0, outlen);
    ret = iconv(cd, &inbuf, (size_t *)&inlen, &outbuf, (size_t *)&outlen);

    iconv_close(cd);

    return ret;
}

char *gb2utf8(char *putf8, int utf8len, char *gb, int gblen)
{
    if (code_convert("GB2312", "utf-8", gb, gblen, putf8, utf8len) > -1)
    {
        return putf8;
    }
    return NULL;
}
gboolean IsChinese(void)
{
    char *p = getenv("LANG");

    if ((p[0] == 'z' || p[0] == 'Z') && (p[1] == 'h' || p[1] == 'H') && (p[2] == '_') && (p[3] == 'C' || p[3] == 'c') && (p[4] == 'N' || p[4] == 'n'))
    {
        return TRUE;
    }
    return FALSE;
}
gchar *trantext(gchar *en, gchar *cn)
{
    return IsChinese() ? cn : en;
}

gint msgbox(GtkWindow *parent, gchar *text, gint type)
{
    gint result;
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                    type, // GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK,
                                    text);
    result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return result;
}

void Sleep(int ms)
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = ms * 1000;
    select(0, NULL, NULL, NULL, &tv);
}

void file_ok_sel(GtkWidget *widget, GtkWidget *filew)
{
    guchar *pBuf;
    gint n = 0;

    pBuf = (guchar *)malloc(DEFAULT_WIDTH * DEFAULT_HEIGHT);
    g_pCaptureBuf = pBuf;
    while (g_pCaptureBuf != NULL)
    {
        Sleep(50);
        n++;
        if (n > 100)
        {
            break;
        }
    }
    if (n < 100)
    {
        gint type = -1;
        const gchar *pfile;
        gchar str[512];

        pfile = gtk_file_selection_get_filename(GTK_FILE_SELECTION(filew));
        if (strstr(pfile, ".bmp") != NULL || strstr(pfile, ".BMP") != NULL)
        {
            type = 0;
        }

        if (save_bmp_file(pBuf, DEFAULT_WIDTH, DEFAULT_HEIGHT, pfile, 8))
        {
            printf("Save %s success.\n", pfile);
        }
        else
        {
            sprintf(str, _T("Save %s fail.", "保存图像 %s 错误"), pfile);
            msgbox(g_main_win, str, GTK_MESSAGE_INFO);
        }
    }
    else
    {
        msgbox(g_main_win, _T("Capture image failed", "获取图像失败"), GTK_MESSAGE_INFO);
    }
    free(pBuf);

    gtk_widget_destroy(filew);

    if (g_PauseCount > 0)
    {
        g_PauseCount--;
    }
}

void file_cancel_sel(GtkWidget *widget, GtkWidget *filew)
{
    g_PauseCount = 0;
}

void setcolor(Display *display, Colormap cmap, GC gc, char *name)
{
    XColor color;

    //
    if (!XParseColor(display, cmap, name, &color))
    {
        printf("Cannot parse color name:%s\n", name);
        exit(1);
    }
    //
    if (!XAllocColor(display, cmap, &color))
    {
        printf("Cannot allocate color\n");
        exit(1);
    }
    //
    XSetForeground(display, gc, color.pixel);
}

void open_scanner(void)
{
    gint ret;
    pthread_attr_t attr;
    struct sched_param param;
    FP_RECT rct;

    int Brightness = 0, Contrast = 0, FrameRate = 0;

    LS_GetBright(0, &Brightness);
    LS_GetContrast(0, &Contrast);
    LS_GetFrameRate(0, &FrameRate);

    if (FrameRate < 5 || FrameRate > 30)
    {
        msgbox(g_main_win, _T("Frame rate error", "读取帧速错误"), GTK_MESSAGE_INFO);
        return;
    }

    LS_SetBright(0, Brightness * 15 / FrameRate + 4); // The expressions no right
    LS_SetContrast(0, Contrast * 15 / FrameRate + 4); // The expressions no right
    LS_SetFrameRate(0, 15);

    g_PauseCont = 0;
    // GDK_WINDOW_XWINDOW(GTK_WIDGET(g_main_win)->window)
    //
    rct.left = 6; // GTK_WIDGET(g_ImgFrame)->allocation.x + 8;
    rct.top = 6;  // GTK_WIDGET(g_ImgFrame)->allocation.y + 20;
    rct.right = rct.left + g_disp_width;
    rct.bottom = rct.top + g_disp_height;
    g_phDisp = begin_show_bmp(GTK_WIDGET(g_ImgFrame), &rct, DEFAULT_WIDTH, DEFAULT_HEIGHT, 8);

    sem_init(&g_sem, 0, 0);

    g_terminate_prcs = 0;
    g_terminate_dsp = 0;

    // 设置线程优先级，1-99， 99优先级最高
    pthread_attr_init(&attr);
    pthread_attr_getschedparam(&attr, &param);
    // dbgmsg("param.sched_priority =%d\n", param.sched_priority);
    //	pthread_attr_setschedpolicy(&attr,SCHED_RR);
    param.sched_priority = 9;
    pthread_attr_setschedparam(&attr, &param);
    //  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&g_tprcsid, &attr, process_thread, NULL);
    if (ret != 0)
    {
        msgbox(g_main_win, _T("Create thread error", "创建线程错误"), GTK_MESSAGE_INFO);
        end_show_bmp_data(g_phDisp);
        return;
    }
    ret = pthread_create(&g_tdspid, &attr, display_thread, NULL);
    if (ret != 0)
    {
        msgbox(g_main_win, _T("Create thread error", "创建线程错误"), GTK_MESSAGE_INFO);
        return;
    }

    g_bInit = 1;
}

void callback(GtkWidget *widget, gpointer data)
{
    if (strcmp((gchar *)data, "Mosaic") == 0)
    {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
        {
            g_nRollMode = 3;
            if (g_bInit)
            {
                MC_SetRollMode(g_nRollMode);
            }
        }
        else
        {
            g_nRollMode = 4;
        }
    }
    else if (strcmp((gchar *)data, "Open") == 0)
    {

        if (g_bInit == 0)
        {
            int nRet;
            g_phDisp = NULL;
            g_PauseCount = 0;
            if (!load_dll())
            {
                msgbox(g_main_win, _T("Failed load library", "载入动态库失败"), GTK_MESSAGE_INFO);
                return;
            }

            nRet = LS_Init();
            if (nRet >= 0)
            {
                int w, h;
                LS_GetMaxImageSize(0, &w, &h);
                g_pImgBuf = (guchar *)malloc(w * h);
                if (g_pImgBuf != NULL)
                {
                    open_scanner();
                }
            }
            else
            {
                msgbox(g_main_win, _T("Not find fingerprint scanner", "采集仪没有连接"), GTK_MESSAGE_INFO);
            }
        }
    }
    else if (strcmp((gchar *)data, "Close") == 0)
    {
        g_terminate_dsp = 1;
        g_terminate_prcs = 1;

        if (g_bInit)
        {

            sem_destroy(&g_sem);

            if (g_tprcsid != 0)
            {
                //	pthread_cancel(g_tprcsid);
                // pthread_kill(g_tprcsid);
                pthread_join(g_tprcsid, NULL);
                g_tprcsid = 0;
            }
            if (g_tdspid != 0)
            {
                pthread_join(g_tdspid, NULL);
                g_tdspid = 0;
            }
            if (g_pdlGals != NULL)
            {
                LS_Close();
            }

            end_show_bmp_data(g_phDisp);

            if (g_pImgBuf != NULL)
            {
                free(g_pImgBuf);
                g_pImgBuf = NULL;
            }
            //     pthread_cond_destroy(&g_cond);
            //     memset(&g_cond, 0, sizeof(g_cond));
            g_bInit = 0;
        }
        if (g_pdlGals != NULL)
        {
            dlclose(g_pdlGals);
            g_pdlGals = NULL;
        }
        if (g_pdlGamc != NULL)
        {
            dlclose(g_pdlGamc);
            g_pdlGamc = NULL;
        }
    }
    else if (strcmp((gchar *)data, "Attribute") == 0)
    {
        if (g_bInit)
        {
            LS_Setup();
        }
    }
    else if (strcmp((gchar *)data, "Capture") == 0)
    {
        gint i;
        GtkWidget *filew;

        if (!g_bInit)
            return;
        g_PauseCount++;

        for (i = 1; i < 10000; i++)
        {
            sprintf(&g_exepath[g_exepathlen], "finger_%d.bmp", i);
            if (access(g_exepath, F_OK) != 0)
            {
                break;
            }
        }

        // MyFp_Pause(1);
        filew = gtk_file_selection_new("File selection");
        gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filew)->ok_button),
                           "clicked", (GtkSignalFunc)file_ok_sel, filew);
        gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(filew)->cancel_button),
                                  "clicked", (GtkSignalFunc)file_cancel_sel, filew);
        gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(filew)->cancel_button),
                                  "clicked", (GtkSignalFunc)gtk_widget_destroy, filew);

        gtk_file_selection_set_filename(GTK_FILE_SELECTION(filew), g_exepath);
        gtk_widget_show(filew);
    }
    else if (strcmp((gchar *)data, "Pause") == 0)
    {
        if (g_bInit)
        {
            g_PauseCont = g_PauseCont > 0 ? 0 : 1;
        }
    }
    else if (strcmp((gchar *)data, "Exit") == 0)
    {
        callback(NULL, "Close");
        gtk_main_quit();
    }
}

gint delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    callback(NULL, "Close");
    gtk_main_quit();
    return FALSE;
}

void create_btn(GtkWidget *box, gchar *caption, GCallback callback, gpointer data)
{
    GtkWidget *btn;

    btn = gtk_button_new_with_label(caption);
    gtk_widget_set_size_request(GTK_WIDGET(btn), 72, -1);
    g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(callback), data);
    gtk_box_pack_start(GTK_BOX(box), btn, FALSE, FALSE, 3);
}

int main(int argc, char *argv[])
{
    gint n;
    GtkWidget *window;
    GtkWidget *box1;
    GtkWidget *box2;
    GtkWidget *box3;
    GtkWidget *checkbtn;
    GtkWidget *status_bar;
    GtkWidget *ImgFrame;
    GdkScreen *screen;
    gint screen_h;

    get_exe_path(g_exepath, 512);
    g_exepathlen = strlen(g_exepath);

    // if (!g_thread_supported())
    {
        g_thread_init(NULL);
    }
    gdk_threads_init();

    gtk_init(&argc, &argv);

    screen = gdk_screen_get_default();
    screen_h = gdk_screen_get_height(screen);
    if (screen_h > 700)
    {
        g_disp_width = DEFAULT_WIDTH;
        g_disp_height = DEFAULT_HEIGHT;
    }
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_main_win = GTK_WINDOW(window);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(window), 2);
    gtk_window_set_title(GTK_WINDOW(window), _T(TITLEEN, TITLECH));
    strcpy(g_exepath + g_exepathlen, PNGFILE);
    gtk_window_set_icon_from_file(GTK_WINDOW(window), g_exepath, NULL);
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(delete_event), NULL);

    box1 = gtk_vbox_new(FALSE, 6);
    gtk_container_add(GTK_CONTAINER(window), box1);

    // StatusBar
    status_bar = gtk_statusbar_new();
    gtk_box_pack_end(GTK_BOX(box1), status_bar, TRUE, TRUE, 0);
    n = gtk_statusbar_get_context_id(GTK_STATUSBAR(status_bar), "");
    gtk_statusbar_push(GTK_STATUSBAR(status_bar), n, _T(CPRTEN, CPRTCH));

    //
    box2 = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_end(GTK_BOX(box1), box2, TRUE, TRUE, 0);

    //
    box3 = gtk_vbox_new(FALSE, 2);
    gtk_box_pack_start(GTK_BOX(box2), box3, FALSE, FALSE, 1);
    gtk_container_set_border_width(GTK_CONTAINER(box3), 4);

    box1 = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(box3), box1, FALSE, FALSE, 4);

    //
    box1 = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box2), box1, TRUE, TRUE, 4);
    ImgFrame = gtk_frame_new(_T("Image", "图像"));
    gtk_box_pack_start(GTK_BOX(box1), ImgFrame, TRUE, TRUE, 0);
    gtk_frame_set_shadow_type(GTK_FRAME(ImgFrame), GTK_SHADOW_IN);
    gtk_widget_set_size_request(GTK_WIDGET(ImgFrame), g_disp_width + 16, g_disp_height + 30);
    g_ImgFrame = ImgFrame;

    //
    //
    checkbtn = gtk_check_button_new_with_label(_T("Mosaic", "拼接"));
    gtk_widget_set_size_request(GTK_WIDGET(checkbtn), 65, -1);
    g_signal_connect(G_OBJECT(checkbtn), "clicked", G_CALLBACK(callback), "Mosaic");
    gtk_box_pack_start(GTK_BOX(box3), checkbtn, FALSE, FALSE, 3);
    create_btn(box3, _T("Open", "打开"), G_CALLBACK(callback), "Open");
    create_btn(box3, _T("Save...", "保存..."), G_CALLBACK(callback), "Capture");
    // create_btn(box3, _T("Attrib...",    "属性..."), G_CALLBACK(callback), "Attribute");
    create_btn(box3, _T("Pause", "暂停"), G_CALLBACK(callback), "Pause");
    create_btn(box3, _T("Close", "关闭"), G_CALLBACK(callback), "Close");
    create_btn(box3, _T("Exit", "退出"), G_CALLBACK(callback), "Exit");

    gtk_widget_show_all(window);

    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();

    return 0;
}
gboolean load_dll()
{
    if (g_pdlGals == NULL)
    {
        char *pszErr;

        int len = strlen(g_exepath);
        strcpy(&g_exepath[g_exepathlen], GALSDLLNAME);

        if ((g_pdlGals = dlopen(g_exepath, RTLD_LAZY)) == NULL) // RTLD_NOW
        {
            g_exepath[len] = 0;
            printf("Failed load library: %s\n", g_exepath);
            printf("Error is: %s\n", dlerror());
            return FALSE;
        }
        g_exepath[len] = 0;
        if ((pszErr = dlerror()) != NULL)
        {
            printf("dlerror()=%s\n", pszErr);
            g_pdlGals = NULL;
            return FALSE;
        }
        LS_Init = (LIVESCAN_Init_)dlsym(g_pdlGals, "LIVESCAN_Init");
        LS_Close = (LIVESCAN_Close_)dlsym(g_pdlGals, "LIVESCAN_Close");
        LS_GetChannelCount = (LIVESCAN_GetChannelCount_)dlsym(g_pdlGals, "LIVESCAN_GetChannelCount");
        LS_SetContrast = (LIVESCAN_SetContrast_)dlsym(g_pdlGals, "LIVESCAN_SetContrast");
        LS_GetBright = (LIVESCAN_GetBright_)dlsym(g_pdlGals, "LIVESCAN_GetBright");
        LS_SetBright = (LIVESCAN_SetBright_)dlsym(g_pdlGals, "LIVESCAN_SetBright");
        LS_GetContrast = (LIVESCAN_GetContrast_)dlsym(g_pdlGals, "LIVESCAN_GetContrast");
        LS_GetMaxImageSize = (LIVESCAN_GetMaxImageSize_)dlsym(g_pdlGals, "LIVESCAN_GetMaxImageSize");
        LS_GetCaptWindow = (LIVESCAN_GetCaptWindow_)dlsym(g_pdlGals, "LIVESCAN_GetCaptWindow");
        LS_SetCaptWindow = (LIVESCAN_SetCaptWindow_)dlsym(g_pdlGals, "LIVESCAN_SetCaptWindow");
        LS_Setup = (LIVESCAN_Setup_)dlsym(g_pdlGals, "LIVESCAN_Setup");
        LS_GetErrorInfo = (LIVESCAN_GetErrorInfo_)dlsym(g_pdlGals, "LIVESCAN_GetErrorInfo");
        LS_BeginCapture = (LIVESCAN_BeginCapture_)dlsym(g_pdlGals, "LIVESCAN_BeginCapture");
        LS_GetFPRawData = (LIVESCAN_GetFPRawData_)dlsym(g_pdlGals, "LIVESCAN_GetFPRawData");
        LS_EndCapture = (LIVESCAN_EndCapture_)dlsym(g_pdlGals, "LIVESCAN_EndCapture");
        LS_IsSupportCaptWindow = (LIVESCAN_IsSupportCaptWindow_)dlsym(g_pdlGals, "LIVESCAN_IsSupportCaptWindow");
        LS_IsSupportSetup = (LIVESCAN_IsSupportSetup_)dlsym(g_pdlGals, "LIVESCAN_IsSupportSetup");
        LS_GetVersion = (LIVESCAN_GetVersion_)dlsym(g_pdlGals, "LIVESCAN_GetVersion");
        LS_GetDesc = (LIVESCAN_GetDesc_)dlsym(g_pdlGals, "LIVESCAN_GetDesc");
        LS_SetFrameRate = (LIVESCAN_SetFrameRate_)dlsym(g_pdlGals, "LIVESCAN_SetFrameRate");
        LS_GetFrameRate = (LIVESCAN_GetFrameRate_)dlsym(g_pdlGals, "LIVESCAN_GetFrameRate");
        LS_RawToBmp = (LIVESCAN_RawToBmp_)dlsym(g_pdlGals, "LIVESCAN_RawToBmp");
        LS_BmpToRaw = (LIVESCAN_BmpToRaw_)dlsym(g_pdlGals, "LIVESCAN_BmpToRaw");
        if ((pszErr = dlerror()) != NULL)
        {
            printf("dlerror()=%s\n", pszErr);
            dlclose(g_pdlGals);
            g_pdlGals = NULL;

            return FALSE;
        }

        strcpy(&g_exepath[g_exepathlen], GAMCDLLNAME);
        if ((g_pdlGamc = dlopen(g_exepath, RTLD_LAZY)) == NULL) // RTLD_NOW
        {
            g_exepath[len] = 0;
            printf("Failed load library: %s\n", g_exepath);
            printf("Error is: %s\n", dlerror());
            return FALSE;
        }
        g_exepath[len] = 0;
        if ((pszErr = dlerror()) != NULL)
        {
            printf("dlerror()=%s\n", pszErr);
            g_pdlGamc = NULL;
            return FALSE;
        }
        MC_Init = (MOSAIC_Init_)dlsym(g_pdlGamc, "MOSAIC_Init");
        MC_Close = (MOSAIC_Close_)dlsym(g_pdlGamc, "MOSAIC_Close");
        MC_IsSupportIdentifyFinger = (MOSAIC_IsSupportIdentifyFinger_)dlsym(g_pdlGamc, "MOSAIC_IsSupportIdentifyFinger");
        MC_IsSupportImageQuality = (MOSAIC_IsSupportImageQuality_)dlsym(g_pdlGamc, "MOSAIC_IsSupportImageQuality");
        MC_IsSupportFingerQuality = (MOSAIC_IsSupportFingerQuality_)dlsym(g_pdlGamc, "MOSAIC_IsSupportFingerQuality");
        MC_IsSupportImageEnhance = (MOSAIC_IsSupportImageEnhance_)dlsym(g_pdlGamc, "MOSAIC_IsSupportImageEnhance");
        MC_IsSupportRollCap = (MOSAIC_IsSupportRollCap_)dlsym(g_pdlGamc, "MOSAIC_IsSupportRollCap");
        MC_SetRollMode = (MOSAIC_SetRollMode_)dlsym(g_pdlGamc, "MOSAIC_SetRollMode");
        MC_Start = (MOSAIC_Start_)dlsym(g_pdlGamc, "MOSAIC_Start");
        MC_DoMosaic = (MOSAIC_DoMosaic_)dlsym(g_pdlGamc, "MOSAIC_DoMosaic");
        MC_Stop = (MOSAIC_Stop_)dlsym(g_pdlGamc, "MOSAIC_Stop");
        MC_ImageQuality = (MOSAIC_ImageQuality_)dlsym(g_pdlGamc, "MOSAIC_ImageQuality");
        MC_FingerQuality = (MOSAIC_FingerQuality_)dlsym(g_pdlGamc, "MOSAIC_FingerQuality");
        MC_ImageEnhance = (MOSAIC_ImageEnhance_)dlsym(g_pdlGamc, "MOSAIC_ImageEnhance");
        MC_IsFinger = (MOSAIC_IsFinger_)dlsym(g_pdlGamc, "MOSAIC_IsFinger");
        MC_GetErrorInfo = (MOSAIC_GetErrorInfo_)dlsym(g_pdlGamc, "MOSAIC_GetErrorInfo");
        MC_GetVersion = (MOSAIC_GetVersion_)dlsym(g_pdlGamc, "MOSAIC_GetVersion");
        MC_GetDesc = (MOSAIC_GetDesc_)dlsym(g_pdlGamc, "MOSAIC_GetDesc");
        if ((pszErr = dlerror()) != NULL)
        {
            dlclose(g_pdlGamc);
            g_pdlGamc = NULL;

            return FALSE;
        }
    }

    return g_pdlGals != NULL && g_pdlGamc != NULL;
}

//----------------------------------------------------------------------------
void *process_thread(void *param)
{
    BOOL bStartMosaic = FALSE;
    gint Width = 640;
    gint Height = 640;
    BYTE *pCurBuf;
    BYTE *pTmpBuf = NULL;
    gint nRet = -1;

    MC_Init();

    LS_GetMaxImageSize(0, &Width, &Height);
    pTmpBuf = (BYTE *)malloc(Width * Height);
    if (pTmpBuf == NULL)
    {
        printf("Not enough memory\n");
        pthread_exit(NULL);
        return NULL;
    }
    pCurBuf = pTmpBuf;
    while (g_terminate_prcs == 0)
    {
        if (g_PauseCount == 0)
        {
            if (LS_BeginCapture(0) != LS_ERROR_SUCCESS)
            {
                printf("LS_BeginCapture error\n");
                break;
            }
            nRet = LS_GetFPRawData(0, (BYTE *)pTmpBuf);
            LS_EndCapture(0);
        }
        else
        {
            usleep(40000);
            sem_post(&g_sem);
        }
        if (nRet == LS_ERROR_SUCCESS)
        {
            if (g_nRollMode >= 0 && g_nRollMode <= 3 && g_PauseCont == 0)
            {
                if (!bStartMosaic)
                {
                    bStartMosaic = MC_IsFinger(pTmpBuf, Width, Height) > 0;
                    if (bStartMosaic)
                    {
                        MC_Start((BYTE *)g_pImgBuf, Width, Height);
                        pCurBuf = g_pImgBuf;
                    }
                    else
                    {
                        memset(pTmpBuf, 0xFF, Width * Height);
                    }
                }
                else
                {
                    if ((nRet = MC_DoMosaic(pTmpBuf, Width, Height)) <= 0)
                    {
                        bStartMosaic = FALSE;
                        if (nRet == 0)
                        {
                            MC_Stop();
                        }
                        else
                        {
                            pCurBuf = pTmpBuf;
                        }
                    }
                }
            }
            else
            {
                pCurBuf = pTmpBuf;
                bStartMosaic = 0;
            }

            if (g_pImgBuf != NULL)
            {
                if (g_pImgBuf != pCurBuf)
                {
                    if (g_PauseCont == 0)
                    {
                        memcpy(g_pImgBuf, pCurBuf, Width * Height);
                    }
                }
                //      if (g_pImgBuf != NULL && g_terminate_prcs == 0)
                //     {
                //         show_bmp_data(g_pImgBuf);
                //     }

                sem_post(&g_sem);
            }
            if (g_pCaptureBuf != NULL)
            {
                memcpy(g_pCaptureBuf, g_pImgBuf, Width * Height);
                g_pCaptureBuf = NULL;
            }
        }
    }

    MC_Close();

    free(pTmpBuf);
    pthread_exit(NULL);
}

/*-----------------------------------------------------------------------------
函数名   : transfer_thread
功能     : 读取设备信息
参数     : OUT hi - 设备信息
返回值   : 成功返回TRUE, 失败返回FALSE
-----------------------------------------------------------------------------*/
void *display_thread(void *param)
{
    struct timespec ts;
    while (g_terminate_dsp == 0)
    {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 2;
        // 等待复制完成
        if (sem_timedwait(&g_sem, &ts) != 0)
        {
            continue;
        }

        if (g_terminate_dsp != 0)
        {
            printf("display_thread break;\n");
            break;
        }
        // display image
        if (g_pImgBuf != NULL && g_terminate_dsp == 0)
        {
            show_bmp_gray(g_phDisp, g_pImgBuf);
        }
    }
    pthread_exit(NULL);
}
