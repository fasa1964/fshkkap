#include "classprinting.h"

#include <QMessageBox>
#include <QPageLayout>
#include <QPainter>

ClassPrinting::ClassPrinting(QObject *parent) : QObject(parent)
{

}

bool ClassPrinting::setupPrinter()
{
    QPrintDialog printDialog(&printer, nullptr);
    if(printDialog.exec() == QDialog::Rejected )
    {
        emit aborted();
        return false;
    }

    QPageLayout pageLayout = printer.pageLayout();
    qreal top = pixel(15, printer);
    qreal left = pixel(20, printer);
    qreal right = pixel(15, printer);
    qreal bottom = pixel(30, printer);
    QMarginsF ma( left, top, right, bottom);
    margin = ma;
    pageLayout.setOrientation(QPageLayout::Portrait);
    pageLayout.setPageSize(QPageSize(QPageSize::A4));
    pageLayout.setMargins(margin);
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

void ClassPrinting::setColumn(int nr, qreal width)
{
    columnMap.insert(nr, width);
}

bool ClassPrinting::print()
{
    QPainter painter;

    if(!painter.begin(&printer))
        return false;


    // Print the frames and the minimum nesseccery text
    setFontAttribute(1, false, Qt::lightGray, &painter);
    painter.drawRect( headerFrame() );
    painter.drawRect( pageFrame() );
    painter.drawRect( feetFrame() );


    setFontAttribute(14, false, QColor(77, 121, 255), &painter);
    painter.drawText( QPointF( headerFrame().x() + 10, headerFrame().bottom()  - ( textHeight(painter) / 2) ) , getCaption() );

    setFontAttribute(12, false, QColor(0,0,0), &painter);
    painter.drawText( QPointF( headerFrame().right() - textWidth(getCurrentDate() , painter) - 10,
                               headerFrame().bottom()  - ( textHeight(painter) / 2) ), getCurrentDate() );

    setFontAttribute(8, false, Qt::lightGray, &painter);
    painter.drawText( QPointF( feetFrame().x() + 10, feetFrame().bottom() - ( textHeight(painter) / 2) ) , getDocName() );




    painter.end();

    return true;
}

QString ClassPrinting::getDocName() const
{
    return docName;
}

qreal ClassPrinting::pageLeft()
{
    return printer.pageRect().left();
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
    return printer.pageRect().width() - margin.left() - margin.right();
}

qreal ClassPrinting::pageHeight()
{
    return printer.pageRect().height() - margin.top()- margin.bottom();
}

QRectF ClassPrinting::headerFrame()
{
    QRectF rect(pageLeft(), pageTop(), pageWidth(), pixel(45, printer));
    return rect;
}

QRectF ClassPrinting::feetFrame()
{
    QPointF p1;
    p1.setX( pageLeft());
    p1.setY(pageHeight());

    QRectF rect(p1.x(), p1.y(), pageWidth(), pixel(25, printer));
    return rect;
}

QRectF ClassPrinting::pageFrame()
{

    QPointF p1;
    p1.setX( headerFrame().bottomLeft().x() );
    p1.setY( headerFrame().bottomLeft().y()+pixel(10,printer));

    QPointF p2;
    p2.setX( feetFrame().topLeft().x() );
    p2.setY( feetFrame().topLeft().y()-pixel(10,printer));

    qreal h = QLineF(p1, p2).length();
    qreal w = headerFrame().width();
    QRectF rect(p1.x(), p1.y(), w, h);
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

qreal ClassPrinting::pixel(qreal millimeter, const QPrinter &p)
{
    qreal fac = p.pageRect().width() / p.pageRect(QPrinter::Millimeter).width();
    int dpi = p.resolution();
    qreal pixelPerMM = dpi / 25.4;

    return fac * millimeter / pixelPerMM;
}

void ClassPrinting::setFontAttribute(int size, bool bold, QColor col, QPainter *p)
{
    QFont font = p->font();
    QPen pen = p->pen();

    font.setPointSize(size);
    font.setBold(bold);
    pen.setColor(col);

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


