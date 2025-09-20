#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QResource>
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
#include <QScreen>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QCursor>
#include <QStyle>
#include <QDateTime>

#include <memory>

// 简化的剪贴板监控器类
class ClipboardMonitor : public QObject {
    Q_OBJECT
public:
    explicit ClipboardMonitor(QObject* parent = nullptr)
        : QObject(parent)
        , m_clipboard(QApplication::clipboard())
        , m_debounceTimer(new QTimer(this))
        , m_pollTimer(new QTimer(this))
        , m_isMonitoring(false)
    {
        // 设置防抖定时器
        m_debounceTimer->setSingleShot(true);
        m_debounceTimer->setInterval(300);
        
        // 设置轮询定时器 (Wayland兼容)
        m_pollTimer->setInterval(500); // 每500ms检查一次
        connect(m_pollTimer, &QTimer::timeout, this, &ClipboardMonitor::checkClipboard);
        
        // 连接剪贴板变化信号
        connect(m_clipboard, &QClipboard::dataChanged, this, [this]() {
            qDebug() << "剪贴板信号触发，监控状态:" << m_isMonitoring;
            if (m_isMonitoring) {
                qDebug() << "启动防抖定时器";
                m_debounceTimer->start();
            }
        });
        
        connect(m_debounceTimer, &QTimer::timeout, this, [this]() {
            QString currentContent = m_clipboard->text();
            qDebug() << "防抖定时器触发，当前内容:" << currentContent;
            qDebug() << "上次内容:" << m_lastContent;
            qDebug() << "内容是否为空:" << currentContent.isEmpty();
            qDebug() << "内容是否不同:" << (currentContent != m_lastContent);
            
            // 检查内容是否有效且与上次不同
            if (!currentContent.isEmpty() && currentContent != m_lastContent) {
                m_lastContent = currentContent;
                qDebug() << "发送剪贴板内容变化信号";
                emit clipboardContentChanged(currentContent);
                qDebug() << "检测到剪贴板内容变化:" << currentContent.left(50) << "...";
            } else {
                qDebug() << "跳过重复或空内容";
            }
        });
    }

    void startMonitoring() {
        if (!m_isMonitoring) {
            m_isMonitoring = true;
            m_pollTimer->start(); // 启动轮询
            qDebug() << "剪贴板监控已启动 (轮询模式)";
            qDebug() << "轮询定时器间隔:" << m_pollTimer->interval() << "ms";
            qDebug() << "轮询定时器是否激活:" << m_pollTimer->isActive();
        }
    }

    void stopMonitoring() {
        if (m_isMonitoring) {
            m_isMonitoring = false;
            m_debounceTimer->stop();
            m_pollTimer->stop(); // 停止轮询
            qDebug() << "剪贴板监控已停止";
        }
    }

    bool isMonitoring() const {
        return m_isMonitoring;
    }

signals:
    void clipboardContentChanged(const QString& content);

private:
    void checkClipboard() {
        if (!m_isMonitoring) {
            qDebug() << "轮询检查：监控未启用";
            return;
        }
        
        QString currentContent = m_clipboard->text();
        qDebug() << "轮询检查：当前内容=" << currentContent << "，上次内容=" << m_lastContent;
        
        if (!currentContent.isEmpty() && currentContent != m_lastContent) {
            qDebug() << "轮询检测到剪贴板变化:" << currentContent.left(30) << "...";
            m_lastContent = currentContent;
            emit clipboardContentChanged(currentContent);
        }
    }

private:
    QClipboard* m_clipboard;
    QString m_lastContent;
    QTimer* m_debounceTimer;
    QTimer* m_pollTimer;
    bool m_isMonitoring;
};

// 简化的悬浮图标类
class FloatingIcon : public QWidget {
    Q_OBJECT
public:
    explicit FloatingIcon(QWidget* parent = nullptr)
        : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool)
        , m_fadeAnimation(new QPropertyAnimation(this))
        , m_opacityEffect(new QGraphicsOpacityEffect(this))
    {
        setupIcon();
        setGraphicsEffect(m_opacityEffect);
        setFixedSize(32, 32);
        setAttribute(Qt::WA_TranslucentBackground);
        
        // 设置淡入淡出动画
        m_fadeAnimation->setTargetObject(m_opacityEffect);
        m_fadeAnimation->setPropertyName("opacity");
        m_fadeAnimation->setDuration(300);
        m_fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    }

    void setupIcon() {
        // 创建简单的圆形图标
        m_icon = QPixmap(32, 32);
        m_icon.fill(Qt::transparent);
        
        QPainter painter(&m_icon);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // 绘制蓝色圆形背景
        painter.setBrush(QBrush(QColor(0, 123, 255, 200)));
        painter.setPen(QPen(QColor(255, 255, 255, 150), 2));
        painter.drawEllipse(2, 2, 28, 28);
        
        // 绘制分析图标 (简单的"?"符号)
        painter.setPen(QPen(QColor(255, 255, 255), 3));
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "?");
    }

    void showNearCursor(const QString& content = QString()) {
        m_currentContent = content;
        
        // 获取鼠标位置并调整图标位置
        QPoint cursorPos = QCursor::pos();
        QPoint iconPos = cursorPos + QPoint(10, 10);
        
        // 确保图标在屏幕范围内
        QScreen* screen = QApplication::screenAt(cursorPos);
        if (screen) {
            QRect screenGeometry = screen->availableGeometry();
            if (iconPos.x() + 32 > screenGeometry.right()) {
                iconPos.setX(cursorPos.x() - 42);
            }
            if (iconPos.y() + 32 > screenGeometry.bottom()) {
                iconPos.setY(cursorPos.y() - 42);
            }
        }
        
        move(iconPos);
        show();
        animateShow();
        
        // 添加闪烁效果提示用户注意
        QTimer::singleShot(100, this, [this]() {
            animateShow(); // 再次淡入以产生闪烁效果
        });
        
        // 自动隐藏定时器
        QTimer::singleShot(5000, this, &FloatingIcon::hideIcon);
    }

    void hideIcon() {
        animateHide();
    }

    bool isVisible() const {
        return QWidget::isVisible();
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawPixmap(0, 0, m_icon);
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            emit iconClicked(m_currentContent);
            hideIcon();
        }
    }

    void enterEvent(QEnterEvent* event) override {
        // 鼠标悬停时停止自动隐藏
        Q_UNUSED(event)
    }

    void leaveEvent(QEvent* event) override {
        // 鼠标离开时重新启动自动隐藏
        Q_UNUSED(event)
        QTimer::singleShot(3000, this, &FloatingIcon::hideIcon);
    }

    void showEvent(QShowEvent* event) override {
        QWidget::showEvent(event);
    }

signals:
    void iconClicked(const QString& content);

private:
    void animateShow() {
        m_opacityEffect->setOpacity(0.0);
        m_fadeAnimation->setStartValue(0.0);
        m_fadeAnimation->setEndValue(0.9);
        m_fadeAnimation->start();
    }

    void animateHide() {
        m_fadeAnimation->setStartValue(m_opacityEffect->opacity());
        m_fadeAnimation->setEndValue(0.0);
        connect(m_fadeAnimation, &QPropertyAnimation::finished, this, &QWidget::hide);
        m_fadeAnimation->start();
    }

private:
    QPixmap m_icon;
    QString m_currentContent;
    QPropertyAnimation* m_fadeAnimation;
    QGraphicsOpacityEffect* m_opacityEffect;
};

// 简化的主应用程序类
class MainApplication : public QApplication {
    Q_OBJECT
public:
    explicit MainApplication(int argc, char* argv[])
        : QApplication(argc, argv)
        , m_settings(nullptr)
    {
        setApplicationName("FieldExplainer");
        setApplicationVersion("1.0.0");
        setOrganizationName("FieldExplainer");
        setQuitOnLastWindowClosed(false);
    }

    bool initialize() {
        // 加载资源文件
        QResource::registerResource(":/resources.qrc");
        
        // 初始化设置
        loadSettings();
        
        // 创建剪贴板监控器
        m_clipboardMonitor = std::make_unique<ClipboardMonitor>(this);
        
        // 创建悬浮图标
        m_floatingIcon = std::make_unique<FloatingIcon>();
        
        // 连接信号
        connect(m_clipboardMonitor.get(), &ClipboardMonitor::clipboardContentChanged,
                this, [this](const QString& content) {
                    // 过滤有效内容
                    if (content.length() > 1000 || content.isEmpty()) {
                        return;
                    }
                    
                    // 控制台提示
                    qDebug() << "==========================================";
                    qDebug() << "🔍 检测到剪贴板新内容:";
                    qDebug() << "内容:" << content;
                    qDebug() << "长度:" << content.length() << "字符";
                    qDebug() << "时间:" << QDateTime::currentDateTime().toString("hh:mm:ss");
                    qDebug() << "提示: 点击悬浮图标进行分析";
                    qDebug() << "==========================================";
                    
                    // 显示系统托盘提示
                    if (m_systemTray) {
                        QString shortContent = content.length() > 50 ? 
                            content.left(47) + "..." : content;
                        m_systemTray->showMessage(
                            "🔍 检测到新内容", 
                            QString("内容: %1\n\n点击悬浮图标进行分析").arg(shortContent),
                            QSystemTrayIcon::Information, 
                            4000
                        );
                    }
                    
                    // 显示悬浮图标
                    m_floatingIcon->showNearCursor(content);
                });
        
        connect(m_floatingIcon.get(), &FloatingIcon::iconClicked,
                this, [this](const QString& content) {
                    qDebug() << "悬浮图标被点击，内容:" << content;
                    
                    // 这里将来会实现字段分析功能
                    QMessageBox::information(nullptr, "字段分析", 
                                           QString("分析内容: %1\n\n分析功能正在开发中...").arg(content));
                });
        
        // 启动剪贴板监控
        m_clipboardMonitor->startMonitoring();
        
        // 设置系统托盘
        setupSystemTray();
        
        qDebug() << "FieldExplainer 初始化完成";
        return true;
    }

    void showSystemTray() {
        if (m_systemTray) {
            m_systemTray->show();
            m_systemTray->showMessage("FieldExplainer", "应用程序已在后台运行", 
                                      QSystemTrayIcon::Information, 3000);
        }
    }

private:
    void setupSystemTray() {
        // 检查系统托盘是否可用
        if (!QSystemTrayIcon::isSystemTrayAvailable()) {
            QMessageBox::critical(nullptr, "系统错误", "系统托盘不可用");
            return;
        }
        
        // 创建托盘图标
        m_systemTray = std::make_unique<QSystemTrayIcon>(this);
        m_systemTray->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
        
        // 创建托盘菜单
        m_trayMenu = std::make_unique<QMenu>();
        
        QAction* quitAction = m_trayMenu->addAction("退出");
        connect(quitAction, &QAction::triggered, this, [this]() {
            saveSettings();
            quit();
        });
        
        m_systemTray->setContextMenu(m_trayMenu.get());
        m_systemTray->show();
    }

    void loadSettings() {
        QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        QDir().mkpath(configPath);
        
        m_settings = new QSettings(QString("%1/%2.ini").arg(configPath, applicationName()), 
                                   QSettings::IniFormat, this);
    }

    void saveSettings() {
        if (m_settings) {
            m_settings->sync();
        }
    }

private:
    std::unique_ptr<ClipboardMonitor> m_clipboardMonitor;
    std::unique_ptr<FloatingIcon> m_floatingIcon;
    std::unique_ptr<QSystemTrayIcon> m_systemTray;
    std::unique_ptr<QMenu> m_trayMenu;
    QSettings* m_settings;
    QString m_lastAnalyzedContent;
};

// 主函数
int main(int argc, char *argv[]) {
    MainApplication app(argc, argv);
    
    if (!app.initialize()) {
        qCritical() << "应用程序初始化失败";
        return -1;
    }
    
    app.showSystemTray();
    
    return app.exec();
}

#include "main.moc"