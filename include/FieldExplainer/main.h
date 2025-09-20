#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QClipboard>
#include <QTimer>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QResource>
#include <QStyle>
#include <QScreen>
#include <QApplication>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>

#include <memory>

namespace FieldExplainer {

// 剪贴板监控器类
class ClipboardMonitor : public QObject {

public:
    explicit ClipboardMonitor(QObject* parent = nullptr);
    ~ClipboardMonitor() override = default;

    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;

signals:
    void clipboardContentChanged(const QString& content);
    void clipboardEmpty();

private slots:
    void onClipboardChanged();
    void onDebounceTimeout();

private:
    QClipboard* m_clipboard;
    QString m_lastContent;
    QTimer* m_debounceTimer;
    bool m_isMonitoring;
    static constexpr int DEBOUNCE_DELAY_MS = 300;
};

// 悬浮图标类
class FloatingIcon : public QWidget {

public:
    explicit FloatingIcon(QWidget* parent = nullptr);
    ~FloatingIcon() override = default;

    void showNearCursor(const QString& content = QString());
    void hideIcon();
    bool isVisible() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void showEvent(QShowEvent* event) override;

signals:
    void iconClicked(const QString& content);

private:
    void setupIcon();
    void animateShow();
    void animateHide();
    QPoint getCursorPosition();

private:
    QPixmap m_icon;
    QString m_currentContent;
    QPropertyAnimation* m_fadeAnimation;
    QGraphicsOpacityEffect* m_opacityEffect;
    static constexpr int ICON_SIZE = 32;
    static constexpr int ICON_MARGIN = 10;
    static constexpr int AUTO_HIDE_DELAY_MS = 3000;
};

// 主应用程序类
class MainApplication : public QApplication {

public:
    explicit MainApplication(int argc, char* argv[]);
    ~MainApplication() override = default;

    bool initialize();
    void showSystemTray();

private slots:
    void onClipboardContentChanged(const QString& content);
    void onFloatingIconClicked(const QString& content);
    void onQuitAction();

private:
    void setupSystemTray();
    void loadSettings();
    void saveSettings();
    QPoint getOptimalIconPosition();

private:
    std::unique_ptr<ClipboardMonitor> m_clipboardMonitor;
    std::unique_ptr<FloatingIcon> m_floatingIcon;
    std::unique_ptr<QSystemTrayIcon> m_systemTray;
    std::unique_ptr<QMenu> m_trayMenu;
    QSettings* m_settings;
    QString m_lastAnalyzedContent;
};

// 工具函数
namespace Utils {
    QString getClipboardContent();
    void setClipboardContent(const QString& content);
    bool isContentValid(const QString& content);
    QString sanitizeContent(const QString& content);
    QPoint getCursorPosition();
}

} // namespace FieldExplainer
