#include "doubanfm.h"
#include "buttonlabel.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QMouseEvent>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QSlider>

DoubanFM::DoubanFM()
{
    picture = new ButtonLabel;
    picture->setFixedWidth(245);
    picture->installEventFilter(this);
    picture->setStyleSheet("background-color:red;");
    pause = new ButtonLabel;
    pause->setNormalImage(QPixmap(":/images/resource/images/pause.png"));
    like = new ButtonLabel;
    like->setNormalImage(QPixmap(":/images/resource/images/like.png"));
    like->setHoverImage(QPixmap(":/images/resource/images/like-hover.png"));
    trash = new ButtonLabel;
    trash->setNormalImage(QPixmap(":/images/resource/images/remove.png"));
    trash->setHoverImage(QPixmap(":/images/resource/images/remove-hover.png"));
    next = new ButtonLabel;
    next->setNormalImage(QPixmap(":/images/resource/images/next.png"));
    next->setHoverImage(QPixmap(":/images/resource/images/next-hover.png"));
    volumeIcon = new ButtonLabel;
    volumeIcon->setNormalImage(QPixmap(":/images/resource/images/audio.png"));
    volumeIcon->setFixedHeight(10);
    volumeIcon->installEventFilter(this);
    layricTips = new ButtonLabel(picture);
    layricTips->setText(tr("Click to show lyrics"));
    layricTips->setVisible(false);
    layricTips->setStyleSheet("QLabel {"
                                "padding:4px 6px;"
                                "background-color:rgba(66, 66, 66, .6);"
                                "border-radius:4px;"
                              "}");

    timeAxis = new QProgressBar();
    timeAxis->setOrientation(Qt::Horizontal);
    timeAxis->setFixedHeight(2);
    timeAxis->setStyleSheet("QProgressBar { \
                                 background-color:#ddd; \
                                 border:none; \
                             } \
                             QProgressBar::chunk { \
                                 background-color:#9DD6C5; \
                                 border:none; \
                             }");

    volumeSlider = new QSlider;
    volumeSlider->setOrientation(Qt::Horizontal);
    volumeSlider->setFixedHeight(3);
    volumeSlider->setMaximumWidth(0);
    volumeSlider->installEventFilter(this);
    volumeSlider->setStyleSheet("QSlider::handle:horizontal { \
                                     background-color:transparent; \
                                 } \
                                 QSlider::sub-page { \
                                     background-color:black; \
                                 } \
                                 QSlider::add-page { \
                                     background-color:#ddd; \
                                 }");

    volumeAnimation = new QPropertyAnimation(volumeSlider, "maximumWidth");

    artist = new QLabel;
    artist->setStyleSheet("font-size:24px;");
    album = new QLabel;
    album->setStyleSheet("font-size:13px;");
    songName = new QLabel;
    songName->setStyleSheet("font-size:15px;");
    time = new QLabel;
    time->setAlignment(Qt::AlignCenter);
#ifdef QT_DEBUG
    artist->setText("Artist");
    album->setText("< Album >");
    songName->setText("SongName");
    time->setText("-0:00");
#endif

    QHBoxLayout *centerCtrlLayout = new QHBoxLayout;
    centerCtrlLayout->addStretch();
    centerCtrlLayout->addWidget(time);
    centerCtrlLayout->setAlignment(time, Qt::AlignVCenter);
    centerCtrlLayout->addWidget(volumeIcon);
    centerCtrlLayout->setAlignment(volumeIcon, Qt::AlignVCenter);
    centerCtrlLayout->addWidget(volumeSlider);
    centerCtrlLayout->setAlignment(volumeSlider, Qt::AlignVCenter);
    centerCtrlLayout->setSpacing(5);
    centerCtrlLayout->setMargin(0);

    QHBoxLayout *bottomCtrlLayout = new QHBoxLayout;
    bottomCtrlLayout->addStretch();
    bottomCtrlLayout->addWidget(like);
    bottomCtrlLayout->addWidget(trash);
    bottomCtrlLayout->addWidget(next);
    bottomCtrlLayout->setMargin(0);
    bottomCtrlLayout->setSpacing(20);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(pause);
    rightLayout->setAlignment(pause, Qt::AlignRight);
    rightLayout->addWidget(artist);
    rightLayout->addWidget(album);
    rightLayout->addSpacing(15);
    rightLayout->addWidget(songName);
    rightLayout->addSpacing(3);
    rightLayout->addWidget(timeAxis);
    rightLayout->addLayout(centerCtrlLayout);
    rightLayout->addSpacing(45);
    rightLayout->addLayout(bottomCtrlLayout);
    rightLayout->addStretch();
    rightLayout->setSpacing(0);
    rightLayout->setContentsMargins(15, 0, 15, 0);

    QWidget *rightWidget = new QWidget;
    rightWidget->setLayout(rightLayout);
    rightWidget->setStyleSheet("background-color:white;");

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(picture);
    mainLayout->addWidget(rightWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(30, 26, 30, 34);

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect;
    shadowEffect->setBlurRadius(30.0);
    shadowEffect->setColor(Qt::black);
    shadowEffect->setOffset(0, 4);

    setLayout(mainLayout);
    setFixedSize(570, 305);
    setGraphicsEffect(shadowEffect);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowTitle(tr("Douban FM"));
    setWindowIcon(QIcon(":/images/resource/images/doubanFM.png"));
    move(QApplication::desktop()->screen()->rect().center() - rect().center());

    connect(layricTips, &ButtonLabel::clicked, picture, &ButtonLabel::clicked);
    connect(picture, &ButtonLabel::clicked, this, &DoubanFM::toggleLayricsWindow);
}

DoubanFM::~DoubanFM()
{

}

void DoubanFM::mousePressEvent(QMouseEvent *e)
{
    mousePressed = true;
    mousePressPoint = e->pos();
}

void DoubanFM::mouseMoveEvent(QMouseEvent *e)
{
    if (!mousePressed)
        return;

    move(e->pos() - mousePressPoint + pos());
}

void DoubanFM::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

    mousePressed = false;
}

bool DoubanFM::eventFilter(QObject *o, QEvent *e)
{
    if (o == volumeIcon && e->type() == QEvent::Enter)
        showVolumeSlider();
    else if (o == volumeSlider && e->type() == QEvent::Leave)
        hideVolumeSlider();
    else if (o == picture && e->type() == QEvent::Enter) {
        layricTips->show();
        layricTips->move(picture->rect().center() - layricTips->rect().center());
    } else if (o == picture && e->type() == QEvent::Leave)
        layricTips->hide();

    return false;
}

void DoubanFM::showVolumeSlider()
{
    if (volumeAnimation->state() == QPropertyAnimation::Running ||
        volumeSlider->width() != 0)
        return;

    volumeAnimation->setStartValue(0);
    volumeAnimation->setEndValue(60);
    volumeAnimation->start();
}

void DoubanFM::hideVolumeSlider()
{
    if (volumeAnimation->state() == QPropertyAnimation::Running ||
        volumeSlider->width() == 0)
        return;

    volumeAnimation->setStartValue(60);
    volumeAnimation->setEndValue(0);
    volumeAnimation->start();
}

void DoubanFM::toggleLayricsWindow()
{
    qDebug() << "toggle layrics";
}
