#ifndef THEMEPROVIDER_H
#define THEMEPROVIDER_H

#include <QGuiApplication>
#include <QPalette>
#include <QStyleHints>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class ThemeProvider
{
public:
    ThemeProvider();

    static QPalette light(){
        QPalette pal;

        // Backgrounds
        pal.setColor(QPalette::Window,        QColor("#FFFFFF"));
        pal.setColor(QPalette::Base,          QColor("#FFFFFF"));
        pal.setColor(QPalette::AlternateBase, QColor("#F5F3FA"));
        pal.setColor(QPalette::Button,        QColor("#F0EDF5"));

        // Text
        pal.setColor(QPalette::WindowText,    QColor("#1A181E"));
        pal.setColor(QPalette::Text,          QColor("#1A181E"));
        pal.setColor(QPalette::ButtonText,    QColor("#1A181E"));
        pal.setColor(QPalette::PlaceholderText, QColor("#7E788A"));
        pal.setColor(QPalette::BrightText,    QColor("#FFFFFF"));

        // Borders
        pal.setColor(QPalette::Light,         QColor("#FFFFFF"));
        pal.setColor(QPalette::Midlight,      QColor("#E8E5EE"));
        pal.setColor(QPalette::Mid,           QColor("#D0CBD8"));
        pal.setColor(QPalette::Dark,          QColor("#9E97AA"));
        pal.setColor(QPalette::Shadow,        QColor("#C4BECF"));

        // Selection
        pal.setColor(QPalette::Highlight,     QColor("#E4DCF8"));
        pal.setColor(QPalette::HighlightedText, QColor("#1A181E"));
        pal.setColor(QPalette::Accent,        QColor("#7537F1"));

        // Links
        pal.setColor(QPalette::Link,          QColor("#005FB8"));
        pal.setColor(QPalette::LinkVisited,   QColor("#7B0796"));

        return pal;
    };

    static QPalette dark(){
        QPalette pal;

        // Backgrounds
        pal.setColor(QPalette::Window,        QColor("#141216"));
        pal.setColor(QPalette::Base,          QColor("#141216"));
        pal.setColor(QPalette::AlternateBase, QColor("#1E1B22"));
        pal.setColor(QPalette::Button,        QColor("#26222C"));

        // Text
        pal.setColor(QPalette::WindowText,    QColor("#EDEDF2"));
        pal.setColor(QPalette::Text,          QColor("#EDEDF2"));
        pal.setColor(QPalette::ButtonText,    QColor("#EDEDF2"));
        pal.setColor(QPalette::PlaceholderText, QColor("#8E8899"));
        pal.setColor(QPalette::BrightText,    QColor("#FFFFFF"));

        // Borders
        pal.setColor(QPalette::Light,         QColor("#2B2732"));
        pal.setColor(QPalette::Midlight,      QColor("#383241"));
        pal.setColor(QPalette::Mid,           QColor("#484153"));
        pal.setColor(QPalette::Dark,          QColor("#5D556B"));
        pal.setColor(QPalette::Shadow,        QColor("#100E12"));

        // Selection
        pal.setColor(QPalette::Highlight,     QColor("#4C2D91"));
        pal.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
        pal.setColor(QPalette::Accent,        QColor("#9E6CFF"));

        // Links
        pal.setColor(QPalette::Link,          QColor("#4A90E2"));
        pal.setColor(QPalette::LinkVisited,   QColor("#B76CD8"));

        return pal;
    };

    static bool isDark()
    {
#if defined(Q_OS_WIN)
        // ---------------------------------------------------------------------
        // WINDOWS THEME DETECTION:
        // On Windows 10 & 11, the user's preference for Light vs Dark app theme
        // is stored in the registry under:
        //   HKCU\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize
        //   -> AppsUseLightTheme (DWORD: 0 = Dark, 1 = Light)
        // Querying this directly is authoritative and immediate, avoiding any
        // latency or 'Unknown' colorScheme states in Qt QPA.
        //
        // HOW TO TEST ON WINDOWS:
        // Open Windows Settings -> Personalization -> Colors -> Choose your mode.
        // Toggle between 'Light' and 'Dark'.
        // ---------------------------------------------------------------------
        DWORD value = 1;
        DWORD size = sizeof(value);
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER,
                          L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                          0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            LONG res = RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr,
                                         reinterpret_cast<LPBYTE>(&value), &size);
            RegCloseKey(hKey);
            if (res == ERROR_SUCCESS) {
                return (value == 0);
            }
        }
#endif

#if defined(Q_OS_MACOS)
        // ---------------------------------------------------------------------
        // MACOS THEME DETECTION:
        // On macOS 10.14+, Qt 6 queries [NSApp effectiveAppearance].
        // If it equals NSAppearanceNameDarkAqua, Qt styleHints reports
        // Qt::ColorScheme::Dark.
        //
        // HOW TO TEST ON MACOS:
        // System Settings -> Appearance -> Select 'Light' or 'Dark'.
        // Or in Terminal:
        //   osascript -e 'tell app "System Events" to tell appearance preferences to set dark mode to not dark mode'
        // ---------------------------------------------------------------------
#endif

#if defined(Q_OS_LINUX)
        // ---------------------------------------------------------------------
        // LINUX THEME DETECTION:
        // On modern Linux desktop environments (GNOME, KDE Plasma 5/6), Qt 6
        // checks the FreeDesktop XDG Desktop Portal:
        //   org.freedesktop.appearance.color-scheme (1 = Dark, 2 = Light)
        // or GNOME gsettings interface.
        //
        // HOW TO TEST ON LINUX:
        // GNOME Settings -> Appearance -> Dark / Light.
        // Or in Terminal:
        //   gsettings set org.gnome.desktop.interface color-scheme 'prefer-dark'
        //   gsettings set org.gnome.desktop.interface color-scheme 'default'
        // ---------------------------------------------------------------------
#endif

        if (QGuiApplication::styleHints()) {
            return QGuiApplication::styleHints()->colorScheme()
                   == Qt::ColorScheme::Dark;
        }

        return false;
    }

    static bool isTaskbarDark()
    {
#if defined(Q_OS_WIN)
        // SystemUsesLightTheme governs the Windows taskbar, start menu, and notification area
        DWORD value = 1;
        DWORD size = sizeof(value);
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER,
                          L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                          0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            LONG res = RegQueryValueExW(hKey, L"SystemUsesLightTheme", nullptr, nullptr,
                                         reinterpret_cast<LPBYTE>(&value), &size);
            RegCloseKey(hKey);
            if (res == ERROR_SUCCESS) {
                return (value == 0);
            }
        }
#endif
        return isDark();
    }

    static QPalette theme()
    {
        return isDark() ? ThemeProvider::dark() : ThemeProvider::light();
    }

};

#endif // THEMEPROVIDER_H
