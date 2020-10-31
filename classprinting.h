#ifndef CLASSPRINTING_H
#define CLASSPRINTING_H

#include <QObject>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QDateTime>
#include <QMarginsF>
#include <QMap>

class ClassPrinting : public QObject
{
    Q_OBJECT
public:
    explicit ClassPrinting(QObject *parent = nullptr);

    bool setupPrinter();
    void setText(const QString &value);
    QString getText() const;

    void setDocName(const QString &value);
    QString getDocName() const;

    void setCaption(const QString &value);
    QString getCaption() const;

    QString getProjectName() const;
    void setProjectName(const QString &value);

    QString getCreatedDate() const;
    void setCreatedDate(const QString &value);

    QString getDescription() const;
    void setDescription(const QString &value);

    void setColumn(int nr, qreal width);
    void setColumnText(int nr, const QString &text);
    void setColumnRect(int nr, const QRectF &rect);

    bool print();

signals:
    void aborted();

public slots:

private:
    QString docName;
    QString text;
    QString caption;
    QString projectName;
    QString createdDate;
    QString description;

    QPrinter printer;
    QMarginsF margin;

    QMap<int, qreal> columnMap;
    QMap<int, QString> columnText;
    QMap<int, QRectF> columnRect;


    qreal pageLeft();
    qreal pageTop();
    qreal pageBottom();
    qreal pageWidth();
    qreal pageHeight();

    QRectF headerFrame();
    QRectF feetFrame();
    QRectF pageFrame();

    QString getCurrentDate();

    qreal pixel(qreal millimeter, const QPrinter &p);
    void setFontAttribute(int size, bool bold, QColor col, QPainter *p);

    qreal textHeight(const QPainter &p);
    qreal textWidth(const QString &text, const QPainter &p);

};

#endif // CLASSPRINTING_H
