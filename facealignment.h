#ifndef FACEALIGNMENT_H
#define FACEALIGNMENT_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsScene>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtGui/QWheelEvent>

#include "ui_facealignment.h"


class myGraphicsView : public QGraphicsView
{

public:
	myGraphicsView(QWidget * parent = 0) : QGraphicsView(parent) {}
	myGraphicsView(QGraphicsScene * scene, QWidget * parent = 0) : QGraphicsView(scene, parent) {}
	~myGraphicsView(){}

protected:

	virtual void wheelEvent(QWheelEvent *event){

		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

		double scaleFactor = 1.15;

		if (event->delta() > 0)
		{
			scale(scaleFactor, scaleFactor);
		} 
		else
		{
			scale(1.0/scaleFactor, 1.0 / scaleFactor);
		}

	}

};


class myGraphicsScene : public QGraphicsScene
{
	Q_OBJECT
public:
	myGraphicsScene(QObject * parent = 0) : QGraphicsScene(parent){
		leftEyeX =-1;
		leftEyeY =-1;
		rightEyeX =-1;
		rightEyeY =-1;
	}
	myGraphicsScene(const QRectF & sceneRect, QObject * parent = 0) : 
		QGraphicsScene(sceneRect, parent = 0){
			leftEyeX =-1;
			leftEyeY =-1;
			rightEyeX =-1;
			rightEyeY =-1;
	}
	myGraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject * parent = 0) : 
		QGraphicsScene(x, y, width, height, parent){
			leftEyeX =-1;
			leftEyeY =-1;
			rightEyeX =-1;
			rightEyeY =-1;
	}

	int LeftEyeX()
	{
		return leftEyeX;
	}

	int LeftEyeY()
	{
		return leftEyeY;
	}

	int RightEyeX()
	{
		return rightEyeX;
	}

	int RightEyeY()
	{
		return rightEyeY;
	}

signals: 
	void sceneMousePressed();

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent * event)
	{

		QGraphicsScene::mousePressEvent(event);

		if (event->button() == Qt::MouseButton::LeftButton)
		{
			leftEyeX = event->pos().x();
			leftEyeY = event->pos().y();
		}
		else if (event->button() == Qt::MouseButton::RightButton)
		{
			rightEyeX = event->pos().x();
			rightEyeY = event->pos().y();
		}
		else{}

		emit sceneMousePressed();
	}

private:
	int leftEyeX;
	int leftEyeY;
	int rightEyeX;
	int rightEyeY;
};


class FaceAlignment : public QMainWindow
{
	Q_OBJECT

public:
	FaceAlignment(QWidget *parent = 0);
	~FaceAlignment();
	bool openFile(const QString &);

	
public slots:
void sceneMousePressedHandler();

private slots:
	void loadFile();
	void runAlgorithm();

private:
	void createConnections();
	void createMenus();
	void initializeView();
	
	Ui::FaceAlignmentClass ui;
	QMenu *fileMenu;
	QGraphicsPixmapItem* pixmapItem;
	myGraphicsView *graphicsView;
	myGraphicsScene* scene;
	QImage* image;
};

#endif // FACEALIGNMENT_H
