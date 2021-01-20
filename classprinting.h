#ifndef CLASSPRINTING_H
#define CLASSPRINTING_H

#include <QObject>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QDateTime>
#include <QMarginsF>
#include <QMap>

#include <classprojekt.h>
#include <classskills.h>
#include <classlehrling.h>

class ClassPrinting : public QObject
{
    Q_OBJECT
public:
    explicit ClassPrinting(QObject *parent = nullptr);

    bool setupPrinter(QPageLayout::Orientation layout);

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

    bool print();

    ClassProjekt getProject() const;
    void setProject(const ClassProjekt &project);

    QString getFilepath() const;
    void setFilepath(const QString &value);

    QList<ClassLehrling> getApprenticeList() const;
    void setApprenticeList(const QList<ClassLehrling> &apprenticeList);

    QMap<int, QVariant> getResultMap() const;
    void setResultMap(const QMap<int, QVariant> &resultMap);

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
    QString filepath;

    QPrinter printer;
    QMarginsF margin;
    int maxPage;
    int currentPage;
    qreal originPenSize;

    QMap<int, QPointF> rowMap;
    void setupRowMap(const QPainter &p);

    ClassProjekt m_project;
    QMap<int, QPointF> columnPos;
    QMap<int, QString> columnHeaderText;


    QList<ClassLehrling> m_apprenticeList;
    QMap<int, QVariant> m_resultMap;

    qreal pageLeft();
    qreal pageRight();
    qreal pageTop();
    qreal pageBottom();
    qreal pageWidth();
    qreal pageHeight();
    QRectF pageRect();

    QRectF captionFrame();
    QRectF feetFrame();
    QRectF pageFrame();

    QString getCurrentDate();

    qreal pixel(qreal millimeter);
    void setFontAttribute(int fontSize, qreal penWidth, bool bold, QColor col, QPainter *p);

    qreal textHeight(const QPainter &p);
    qreal textWidth(const QString &text, const QPainter &p);
    void setPenSize(qreal size, QPainter *p);


    QStringList splitText(const QString &text, qreal maxLength, const QPainter &p);
    int calculateRows(const QStringList &textList, qreal rowWidth, const QPainter &p);
    QStringList textList(const ClassProjekt &pro);

    void printPageFrames(QPainter *p);
    void printPageInfoText(QPainter *p);
    void printProjectInfoText(QPainter *p);
    void printProjectText(QPainter *p);
    void printApprenticeList(QPainter *p);
    void printResultMap(QPainter *p);
    bool setupNewPage(QPainter *p);

};

#endif // CLASSPRINTING_H
