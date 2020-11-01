#include "classprinting.h"

#include <QMessageBox>
#include <QPageLayout>
#include <QPainter>
#include <QtMath>
#include <fshkwindow.h>

#include <QDebug>

ClassPrinting::ClassPrinting(QObject *parent) : QObject(parent)
{
    m_project = ClassProjekt();
    currentPage = 1;
    maxPage = 1;
}

bool ClassPrinting::setupPrinter(QPageLayout::Orientation layout)
{
    QPrintDialog printDialog(&printer, nullptr);
    if(printDialog.exec() == QDialog::Rejected )
    {
        emit aborted();
        return false;
    }

    QPageLayout pageLayout = printer.pageLayout();
    pageLayout.setOrientation( layout /*QPageLayout::Portrait*/);
    pageLayout.setPageSize(QPageSize(QPageSize::A4));
    pageLayout.setUnits(QPageLayout::Millimeter);
    pageLayout.setMode(QPageLayout::FullPageMode);
    pageLayout.setLeftMargin(15);
    pageLayout.setTopMargin(10);
    pageLayout.setRightMargin(10);
    pageLayout.setBottomMargin(15);
    printer.setPageLayout( pageLayout );

    return true;

}

void ClassPrinting::setText(const QString &value)
{
    text = value;
}

void ClassPrinting::setDocName(const QString &value)
{
    docName = value;
    if(printer.isValid())
        printer.setDocName(docName);
}

void ClassPrinting::setCaption(const QString &value)
{
    caption = value;
}

QString ClassPrinting::getCaption() const
{
    return caption;
}

QString ClassPrinting::getProjectName() const
{
    return projectName;
}

void ClassPrinting::setProjectName(const QString &value)
{
    projectName = value;
}

QString ClassPrinting::getCreatedDate() const
{
    return createdDate;
}

void ClassPrinting::setCreatedDate(const QString &value)
{
    createdDate = value;
}

QString ClassPrinting::getDescription() const
{
    return description;
}

void ClassPrinting::setDescription(const QString &value)
{
    description = value;
}


bool ClassPrinting::print()
{
    QPainter painter;

    if(!painter.begin(&printer))
        return false;

    setFontAttribute(10, 1.0,false, QColor(0,0,0), &painter);
    setupRowMap(painter);

    // Calculate the page count
    int rowNeeded = calculateRows( textList( getProject() ), 519, painter);
    rowNeeded += 8; // 8 = Head rows and last rows for total result
    maxPage = qCeil( rowNeeded / rowMap.size());
    maxPage++;

    // Print the frames and the minimum nesseccery text
    setFontAttribute(10, 1, false, Qt::darkGray, &painter);
    printPageFrames(&painter);
    printPageInfoText(&painter);

    // Print head project information
    if(getProject().isValid())
    {
//        // Calculate the page count
//        int rowNeeded = calculateRows( textList( getProject() ), 519, painter);
//        rowNeeded += 8; // 8 = Head rows and last rows for total result
//        maxPage = qCeil( rowNeeded / rowMap.size());
//        maxPage++;

        setFontAttribute(10, 1.0, false, QColor(0,0,0), &painter);
        printProjectInfoText(&painter);
        printProjectText(&painter);
    }

    // Print apprentice list
    if(!getApprenticeList().isEmpty())
    {
        setFontAttribute(10, 1, false, QColor(0,0,0), &painter);
        printApprenticeList(&painter);
    }

    painter.end();

    return true;
}

ClassProjekt ClassPrinting::getProject() const
{
    return m_project;
}

void ClassPrinting::setProject(const ClassProjekt &project)
{
    m_project = project;
}

QString ClassPrinting::getFilepath() const
{
    return filepath;
}

void ClassPrinting::setFilepath(const QString &value)
{
    filepath = value;
}

void ClassPrinting::setupRowMap(const QPainter &p)
{
    qreal th = textHeight(p) * 1.5;
    int rows = qFloor( pageFrame().height() / th);

    qreal posY = pageFrame().top() + pixel(5);
    qreal posX = pageFrame().left() + pixel(3);

    for(int i = 0; i < rows; i++)
    {
        QPointF pos;
        pos.setX(posX);
        pos.setY(posY);
        rowMap.insert(i, pos);

        posY += th;
        pos.setY(posY);
    }
}

QList<ClassLehrling> ClassPrinting::getApprenticeList() const
{
    return m_apprenticeList;
}

void ClassPrinting::setApprenticeList(const QList<ClassLehrling> &apprenticeList)
{
    m_apprenticeList = apprenticeList;
}

QString ClassPrinting::getDocName() const
{
    return docName;
}

qreal ClassPrinting::pageLeft()
{
    return printer.pageRect().left();
}

qreal ClassPrinting::pageRight()
{
    return printer.pageRect().right();
}

qreal ClassPrinting::pageTop()
{
    return printer.pageRect().top();
}

qreal ClassPrinting::pageBottom()
{
    return printer.pageRect().bottom();
}

qreal ClassPrinting::pageWidth()
{
    QPageLayout layout = printer.pageLayout();
    int ml = layout.marginsPixels(printer.resolution()).left();
    int mr = layout.marginsPixels(printer.resolution()).right();
    qreal w = printer.pageRect().width() - ml - mr;

    return w;
}

qreal ClassPrinting::pageHeight()
{
    QPageLayout layout = printer.pageLayout();
    int mt = layout.marginsPixels(printer.resolution()).top();
    int mb = layout.marginsPixels(printer.resolution()).bottom();
    qreal h = printer.pageRect().height() - mt - mb;

    return h;
}

QRectF ClassPrinting::pageRect()
{
    QRectF rect(pageLeft(), pageTop(), pageWidth(), pageHeight());
    return rect;
}

QRectF ClassPrinting::captionFrame()
{
    QRectF rect(0, 0, pageWidth(), pixel(10));
    return rect;
}

QRectF ClassPrinting::feetFrame()
{
    QRectF rect(0, pageHeight()-pixel(5), pageWidth(), pixel(5));
    return rect;
}

QRectF ClassPrinting::pageFrame()
{
    qreal h = pageHeight() - captionFrame().height() - pixel(3) - feetFrame().height() - pixel(3);
    QRect rect(0, captionFrame().bottom() + pixel(3), pageWidth(), h );
    return rect;
}

QString ClassPrinting::getCurrentDate()
{
    return QDate::currentDate().toString("dd.MM.yyyy");
}

QString ClassPrinting::getText() const
{
    return text;
}

qreal ClassPrinting::pixel(qreal millimeter)
{
    int dpi = printer.resolution();
    qreal pixelPerMM = dpi / 25.4;

    return millimeter * pixelPerMM;
}

void ClassPrinting::setFontAttribute(int fontSize, qreal penWidth, bool bold, QColor col, QPainter *p)
{
    QFont font = p->font();
    QPen pen = p->pen();

    font.setPointSize(fontSize);
    font.setBold(bold);
    pen.setColor(col);
    pen.setWidth(penWidth);

    p->setFont(font);
    p->setPen(pen);
}

qreal ClassPrinting::textHeight(const QPainter &p)
{
    QFontMetricsF fm(p.font());
    return fm.height();
}

qreal ClassPrinting::textWidth(const QString &text, const QPainter &p)
{
    QFontMetricsF fm(p.font());
    return fm.width(text);
}

void ClassPrinting::setPenSize(qreal size, QPainter *p)
{
    QPen pen = p->pen();
    pen.setWidthF(size);
    p->setPen(pen);
}

QStringList ClassPrinting::splitText(const QString &text, qreal maxLength, const QPainter &p)
{
    QStringList list = text.split(" ");

    QStringList textList;
    QString string = "";

    foreach (QString s, list)
    {
        if( textWidth(string+s+" ", p) < maxLength)
        {
            string.append(s);
            string.append(" ");

        }
        else
        {
            textList << string;
            string.clear();
            string.append(s);
            string.append(" ");
        }
    }

    textList << string;


    return textList;
}

int ClassPrinting::calculateRows(const QStringList &textList, qreal rowWidth, const QPainter &p)
{
    int rows = 0;
    foreach (QString s, textList)
    {
        if( textWidth(s, p) > rowWidth)
            rows += splitText(s, rowWidth, p).size();
        else
            rows++;
    }

    return rows;
}

QStringList ClassPrinting::textList(const ClassProjekt &pro)
{
    QStringList list;
    foreach (ClassFrage q, pro.questionMap().values())
    {
        QString s = "100:";
        s.append(q.question());
        list << s;
    }
    return list;
}

void ClassPrinting::printPageFrames(QPainter *p)
{
    setFontAttribute(10, 1.0, false, Qt::darkGray, p);
    p->drawRect( captionFrame() );
    p->drawRect( pageFrame() );
    p->drawRect( feetFrame() );
}

void ClassPrinting::printPageInfoText(QPainter *p)
{
    // Print caption and date
    setFontAttribute(14, 1.0, false, QColor(77, 121, 255), p);
    p->drawText( QPointF( captionFrame().x() + pixel(3), captionFrame().bottom()  - ( textHeight(*p) / 2) ) , getCaption() );

    setFontAttribute(10, 1.0, false, QColor(0,0,0), p);
    p->drawText( QPointF( captionFrame().right() - textWidth(getCurrentDate() , *p) - pixel(3),
                               captionFrame().bottom()  - ( textHeight(*p) / 2) ), getCurrentDate() );

    // Print page feet information
    setFontAttribute(8, 1.0, false, Qt::darkGray, p);

    QString fileino = getDocName();
    if(getProject().isValid())                          // + ":" + getFilepath(); not needed
        fileino.append(":"+getProject().createTime());
    qreal w = textWidth(fileino, *p);
    p->drawText( QPointF( feetFrame().x() + 10, feetFrame().bottom() - ( textHeight(*p) / 2) ) , fileino );


    QString cp = QString::number(currentPage,10);
    QString mp = QString::number(maxPage,10);
    QString pageCount = "Seite " + cp + " von " + mp;

    qreal posX = w + pixel(10);
    if(posX < feetFrame().width() / 2 - (textWidth(pageCount, *p) / 2))
        posX = feetFrame().width() / 2 - (textWidth(pageCount, *p) / 2);

    p->drawText( QPointF( posX , feetFrame().bottom() - ( textHeight(*p) / 2) ) , pageCount );

}

void ClassPrinting::printProjectInfoText(QPainter *p)
{
    // Print project base info
    setFontAttribute(10, 1, false, QColor(0,0,0), p);
    QPointF row1 = rowMap.value(0);
    p->drawText(row1, getProject().getKey());
    QString dt = "Datum:";
    QRectF rdt( pageFrame().right() - pixel(23), row1.y() - pixel(4),
                  pixel(20), pixel(7));
    p->drawText( pageRect().right() - rdt.width() - textWidth(dt, *p) - pixel(6), row1.y(), dt);

    setFontAttribute(10, 1, false, Qt::darkGray, p);
    p->drawRect( rdt);

    // Print project description
    setFontAttribute(10, 1, false, QColor(0,0,0), p);
    QPointF row2 = rowMap.value(1);
    p->drawText( row2, getDescription());

    // Print a rectangle for skill number
    QPointF row3 = rowMap.value(2);
    setFontAttribute(10, 1, false, Qt::darkGray, p);
    QRectF rnr( pageFrame().right() - pixel(23), row3.y() - pixel(5) - (textHeight(*p)/2),
                  pixel(20), pixel(12));
    p->drawRect( rnr );

    setFontAttribute(10, 1, true, QColor(0,0,0), p);
    QString pn = "Prüfungs-Nr:";
    row3.setX(pageFrame().right() - rnr.width() - textWidth(pn, *p) - pixel(6));
    p->drawText(row3 , pn);

    // Print column for editing the points
    setFontAttribute(10, 1, false, QColor(0,0,0), p);
    QString mp = "MaxPunkte";
    columnHeaderText.insert(3, mp);

    QString p1 = "Prüfer 1";
    columnHeaderText.insert(2, p1);

    QString p2 = "Prüfer 2";
    columnHeaderText.insert(1, p2);

    QString p3 = "Prüfer 3";
    columnHeaderText.insert(0, p3);

    QPointF colPos = rowMap.value(4);
    qreal width = pageRect().right();

    colPos.setX( width );
    for(int i = 0; i < columnHeaderText.size(); i++)
    {
        qreal wt = textWidth( columnHeaderText.value(i), *p) + pixel(3);
        colPos.setX( colPos.x() - wt );
        columnPos.insert(i, colPos);
        p->drawText( colPos, columnHeaderText.value(i));
    }
}

void ClassPrinting::printProjectText(QPainter *p)
{

    int rowNr = 5;

    // Calculate the maximal length of row
    QPointF pos1 = rowMap.value(0);
    pos1.setY(0);
    QPointF pos2 = columnPos.last();
    pos2.setY(0);
    qreal maxRowLength = QLineF(pos1, pos2).length() - textWidth("100.", *p) - pixel(5);


    QMap<int, ClassFrage> qMap;
    qMap = getProject().questionMap();

    QMapIterator<int, ClassFrage> it(qMap);
    while (it.hasNext())
    {
        it.next();

        setFontAttribute(10, 1.0, false, QColor(0,0,0), p);

        QString qu = it.value().question();
        if(!it.value().identifier().isEmpty())
            qu.append(" : " + it.value().identifier());

        QString nr = QString::number(it.value().questionNr(), 10)+".";
        QString mp = QString::number( it.value().maxPoints(), 10);

        // Print number of question
        QPointF pos = rowMap.value( rowNr );
        p->drawText(pos, nr);

        // Print question split when string to long
        QStringList sList = splitText(qu, maxRowLength , *p);
        foreach (QString s, sList)
        {
            pos = rowMap.value(rowNr);
            pos.setX( pos.x() + textWidth("100.", *p) );
            p->drawText(pos, s);

            if(sList.first() == s)
            {
                // Print max points
                pos.setX( columnPos.last().x() );
                p->drawText( pos, mp );

                // Definition of edit rect for points
                setFontAttribute(10, 1, false, Qt::darkGray, p);
                for(int i = 0; i < columnHeaderText.size()-1; i++)
                {
                    QRectF editRect( columnPos.value(i).x(),
                                     pos.y() - (textHeight(*p)/2) - pixel(2),
                                     textWidth(columnHeaderText.value(i), *p),
                                     pixel(6));

                    p->drawRect( editRect );
                }
                setFontAttribute(10, 1, false, QColor(0,0,0), p);
            }

            if(sList.last() != s)
                rowNr++;
        }

        if(rowNr > rowMap.size()-2)
        {
            if(setupNewPage(p)){
                rowNr = 5;
            }else
                ; // emit error

        }
        else
            rowNr++;
    }

    // Print the last rows
    rowNr++;
    if(rowNr > rowMap.size() - 2)
    {
        setupNewPage(p);
        rowNr = 5;
    }

    QPointF pos = rowMap.value(rowNr);
    QString result = "Ergebnis:";
    p->drawText( rowMap.value(rowNr), result);

    setFontAttribute(10, 1, false, Qt::darkGray, p);
    qreal rectWidth = 0;
    for(int i = 0; i < columnHeaderText.size()-1; i++)
    {
        QRectF editRect( columnPos.value(i).x(),
                         pos.y() - (textHeight(*p)/2) - pixel(2),
                         textWidth(columnHeaderText.value(i), *p),
                         pixel(6));

        p->drawRect( editRect );
        rectWidth = editRect.width();
    }

    rowNr++;
    rowNr++;
    if(rowNr > rowMap.size() - 2)
    {
        setupNewPage(p);
        rowNr = 5;
    }

    pos = rowMap.value(rowNr);
    setFontAttribute(10, 1, true, QColor(0,0,0), p);
    QString total = "Gesamt:";
    p->drawText( rowMap.value(rowNr), total);

    setFontAttribute(10, 1, true, Qt::darkGray, p);
    QRectF editRect( columnPos.value(2).x(),
                     pos.y() - (textHeight(*p)/2) - pixel(2),
                     rectWidth * 3 + pixel(6),
                     pixel(6));

    p->drawRect( editRect );

}

void ClassPrinting::printApprenticeList(QPainter *p)
{
    QMap<int, qreal> columnWidth;

    columnHeaderText.insert(0, "Nr.");
    columnWidth.insert(0, pixel(20));

    columnHeaderText.insert(1, "Name");
    columnWidth.insert(1, pixel(45));

    columnHeaderText.insert(2, "Klasse");
    columnWidth.insert(2, pixel(20));

    columnHeaderText.insert(3, "Adresse");
    columnWidth.insert(3, pixel(75));

    columnHeaderText.insert(4, "Telefon");
    columnWidth.insert(4, pixel(40));

    columnHeaderText.insert(5, "Betrieb");
    columnWidth.insert(0, pixel(10));


    int rowNr = 0;         // number of row
    qreal xPos = pixel(3); // start position
    QPointF pos;           // position of current painter

    pos.setY( rowMap.value(rowNr).y() );
    pos.setX( xPos );
    for(int i = 0; i < columnHeaderText.size(); i++)
    {
        QString s = columnHeaderText.value(i);
        p->drawText( pos, s);
        columnPos.insert(i, pos);
        xPos += columnWidth.value(i);
        pos.setX( xPos );
    }


    foreach (ClassLehrling appr, getApprenticeList())
    {
        rowNr++;
        QString nr = QString::number(appr.nr(),10);
        QString name = appr.firstname() + " " + appr.surname();
        QString klasse = appr.educationClass();
        QString betrieb = appr.company();
        QString adress = appr.street() +"," + appr.city();
        QString telefon = appr.phone();

        QPointF pos = rowMap.value(rowNr);

        pos.setX(columnPos.value(0).x());
        p->drawText( pos, nr);

        pos.setX(columnPos.value(1).x());
        p->drawText( pos, name);

        pos.setX(columnPos.value(2).x());
        p->drawText( pos, klasse);

        pos.setX(columnPos.value(3).x());
        p->drawText( pos, adress);

        pos.setX(columnPos.value(4).x());
        p->drawText( pos, telefon);

        pos.setX(columnPos.value(5).x());
        p->drawText( pos, betrieb);

        if(rowNr > rowMap.size() - 2)
        {
            setupNewPage(p);
            rowNr = 0;
        }
    }

}

bool ClassPrinting::setupNewPage(QPainter *p)
{
    if(printer.newPage())
    {
        currentPage++;
        printPageFrames(p);
        printPageInfoText(p);

        if(getProject().isValid())
            printProjectInfoText(p);

        return true;
    }

    return false;
}
