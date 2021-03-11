#include   "ViewerWidget.h"

ViewerWidget::ViewerWidget(QString viewerName, QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	name = viewerName;
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
	}
}
ViewerWidget::~ViewerWidget()
{
	delete painter;
	delete img;
}
void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	update();

	return true;
}
bool ViewerWidget::isEmpty()
{
	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}

//Data function
void ViewerWidget::setPixel(int x, int y, const QColor& color)
{
	if (isInside(x, y)) 
		data[x + y * img->width()] = color.rgb();
	else
		qDebug() << "nechce vykreslit bod" << x << "," << y << "\n";
}
void ViewerWidget::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	if (isInside(x, y)) {
		QColor color(r, g, b);
		setPixel(x, y, color);
	}
}

void ViewerWidget::clear(QColor color)
{
	for (size_t x = 0; x < img->width(); x++)
	{
		for (size_t y = 0; y < img->height(); y++)
		{
			setPixel(x, y, color);
		}
	}
	update();
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}

//kresliace funkcie

void ViewerWidget::usecka_DDA(QPoint A, QPoint B, QColor color) {
	int x, y, x2, y2 ;
	double chyba = 0;
	if (B.y() == A.y()) {
		int delta = abs(A.x() - B.x()), x1;
		if (A.x() < B.x())
			x1 = A.x();
		else
			x1 = B.x();
		for (x = 0; x < delta; x++)
			setPixel(x+x1, A.y(), color);
	}
	else {
		double smernica = (B.y() - A.y()) / (double)(B.x() - A.x());
		double posun=0;
		if (abs(smernica) < 1) {											//riadiaca os x
			if (A.x() < B.x()) {
				x = A.x(); y = A.y(); x2 = B.x();
			}
			else if (A.x() > B.x()) {
				x = B.x(); y = B.y(); x2 = A.x();
			}
			else if (A.x() == B.x()) {
				int delta = abs(A.y() - B.y()), y1;
				if (A.y() < B.y())
					y1 = A.y();
				else
					y1 = B.y();
				for (y = 0; y < delta; y++)
					setPixel(y + y1, A.x(), color);
			}
				
			for (x; x < x2; x++) {
				setPixel(x, (int)y, color);
				posun += smernica;
				if (posun > 0.5) {
					y += (int)smernica;
					if ((int)smernica == 0)
						y++;
					posun += -1;
				}
				if (posun < -0.5) {
					y+=-(int) smernica;
					if ((int)smernica == 0)
						y--;
					posun += 1;
				}
			}
		}
		else {																//riadiaca os y
			if (A.y() > B.y())
				 y = A.y(), x = A.x(), y2 = B.y();
			else if (A.y() < B.y())
				 y = B.y(), x = B.x(), y2 = A.y();
			for (y; y > y2; y--) {
				setPixel(x, (int)y, color);
				posun += 1 / smernica;
				if (posun > 0.5) {
					x--;
					posun += -1;
				}
				if (posun < -0.5) {
					x++;
					posun += 1;
				}
			}
		}
	}
	update();
}

void ViewerWidget::usecka_Bresenham(QPoint A, QPoint B, QColor color) {
	int dx, dy, p, x, y, x2, y2;
	if (B.y() == A.y()) {
		int delta = abs(A.x() - B.x()), x1;
		if (A.x() < B.x())
			x1 = A.x();
		else
			x1 = B.x();
		for (x = 0; x < delta; x++)
			setPixel(x + x1, A.y(), color);
	}
	else {
		double smernica = (B.y() - A.y()) / (double)(B.x() - A.x());
		if (smernica < 1 && smernica >= 0) {
			if (A.x() < B.x()) {
				x = A.x();	y = A.y();	x2 = B.x();
				dx = B.x() - A.x();	dy = B.y() - A.y();
			}
			else if (A.x() > B.x()) {
				x = B.x(); y = B.y(); x2 = A.x();
				dx = A.x() - B.x();	dy = A.y() - B.y();
			}
			p = 2 * dy - dx;
			do {
				if (p > 0)
				{
					y++;
					p += 2 * dy - 2 * dx;
				}
				else
				{
					p = p + 2 * dy;
				}
				x++;
				setPixel(x, y, color);
			} while (x < x2);
		}
		else if (smernica > -1 && smernica < 0) {
			if (A.x() < B.x()) {
				x = A.x();	y = A.y();	x2 = B.x();
				dx = B.x() - A.x();	dy = B.y() - A.y();
			}
			else if (A.x() > B.x()) {
				x = B.x(); y = B.y(); x2 = A.x();
				dx = A.x() - B.x();	dy = A.y() - B.y();
			}
			p = 2 * dy + dx;
			do {
				if (p < 0)
				{
					y--;
					p += 2 * dy + 2 * dx;
				}
				else
				{
					p += 2 * dy;
				}
				x++;
				setPixel(x, y, color);
			} while (x < x2);
		}
		else if (smernica < -1) {
			if (A.y() < B.y()) {
				y = A.y();	x = A.x();	y2 = B.y();
				dy = B.y() - A.y();	dx = B.x() - A.x();
			}
			else if (A.y() > B.y()) {
				y = B.y(); x = B.x(); y2 = A.y();
				dx = A.x() - B.x();	dy = A.y() - B.y();
			}
			p = 2 * dx + dy;
			do {
				if (p < 0)
				{
					x--;
					p += 2 * dx + 2 * dy;
				}
				else
				{
					p += 2 * dx;
				}
				y++;
				setPixel(x, y, color);
			} while (y < y2);
		}
		else if (smernica > 1) {
			if (A.y() < B.y()) {
				y = A.y();	x = A.x();	y2 = B.y();
				dx = B.x() - A.x();	dy = B.y() - A.y();
			}
			else if (A.y() > B.y()) {
				y = B.y(); x = B.x(); y2 = A.y();
				dx = A.x() - B.x();	dy = A.y() - B.y();
			}
			p = 2 * dx - dy;
			do {
				if (p > 0)
				{
					x++;
					p += 2 * dx - 2 * dy;
				}
				else
				{
					p += 2 * dx;
				}
				y++;
				setPixel(x, y, color);
			} while (y < y2);
		}
	}

	update();
}

void ViewerWidget::kruznica(QPoint A, QPoint B, QColor color) {
	int a = abs(A.x() - B.x()), b = abs(A.y() - B.y()), r = sqrt((a * a) + (b * b)),
		p = 1 - r, x = 0, y = r, dvaX = 3, dvaY = 2 * r + 2;
	do {
		setPixel(x+A.x(), y+A.y(), color);
		setPixel(-x + A.x(), y + A.y(), color);
		setPixel(-x + A.x(), -y + A.y(), color);
		setPixel(x + A.x(), -y + A.y(), color);
		setPixel(y + A.x(), x + A.y(), color);
		setPixel(-y + A.x(), x + A.y(), color);
		setPixel(-y + A.x(), -x + A.y(), color);
		setPixel(y + A.x(), -x + A.y(), color);
		if (p > 0) {
			p += -dvaY;
			y--;
			dvaY += -2;
		}
		p += dvaX;
		dvaX += 2;
		x++;
	} while (x < y);
	update();
}

void ViewerWidget::kresliPolygon(QVector<QPoint> body, QColor color, int algo) {
	//QPoint A, B;
	//int i ;
	clear();
	update();

	//usecka s orezanim
	/*if (body[0].x()>0 && body[0].y()>0 && body[1].x() > 0 && body[1].y() > 0 && body[0].x() < img->width() && body[0].y() < img->height() && body[1].x() < img->width() && body[1].y() < img->height()) {
		A.setX(body[0].x());
		A.setY(body[0].y());
		B.setX(body[body.count() - 1].x());
		B.setY(body[body.count() - 1].y());
		if (algo == 0)
			usecka_DDA(A, B, color);
		else if (algo == 1)
			usecka_Bresenham(A, B, color);
		qDebug() << "vykresluje cele\n";
	}
	else {*/





	if (body.count()==2){


		QVector<QPoint> vrcholy;
		QPoint d, n, w, e, A, B, E;
		double tl = 0, tu = 1, wn, dn, t;
		int i = 0;
		E.setX(0);
		E.setY(0);
		vrcholy.push_back(E);
		E.setX(img->width() - 1);
		E.setY(0);
		vrcholy.push_back(E);
		E.setX(img->width() - 1);
		E.setY(img->height() - 1);
		vrcholy.push_back(E);
		E.setX(0);
		E.setY(img->height() - 1);
		vrcholy.push_back(E);
		d.setX(body[1].x() - body[0].x());
		d.setY(body[1].y() - body[0].y());

		if ((body[0].x() > 0 && body[0].y() > 0 && body[0].x() < img->width() && body[0].y() < img->height()) || (body[1].x() > 0 && body[1].y() > 0 && body[1].x() < img->width() && body[1].y() < img->height())) {
			// ak aspon 1 bod je vnutri
			trebakreslit = true;
		}
		else  {
			QPoint priesecnik;
			int x, y, priesecniky=0;
			int x1 = body[0].x(), y1 = body[0].y(), x2 = body[1].x(), y2 = body[1].y(), x3, x4, y3, y4;
			//oba body su vonku, treba zistit, ci usecka presekne platno
			for (i = 0; i < vrcholy.count(); i++) {
				x3 = vrcholy[i].x();
				y3 = vrcholy[i].y();
				x4 = vrcholy[(i + 1) % 4].x();
				y4 = vrcholy[(i + 1) % 4].y();
				float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
				float pre = (x1 * y2 - y1 * x2), post = (x3 * y4 - y3 * x4);
				float x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
				float y = (pre * (y3 - y4) - (y1 - y2) * post) / d;
				priesecnik.setX(x);			//priesecnik priamok
				priesecnik.setY(y);
				int mensieX=body[0].x(), vacsieX=body[1].x(), mensieY=body[0].y(), vacsieY=body[1].y();			//"stvorec" okolo usecky na zistenie, ci je priesecnik naozaj na usecke
				if (body[0].x() > body[1].x()) {
					mensieX = body[1].x();
					vacsieX = body[0].x();
				}
				if (body[0].y() > body[1].y()) {
					mensieY = body[1].y();
					vacsieY = body[0].y();
				}
				if (priesecnik.x() >= 0 && priesecnik.x() < img->width() && priesecnik.y() >= 0 && priesecnik.y() < img->height() && priesecnik.x() >= mensieX && priesecnik.x() < vacsieX && priesecnik.y() >= mensieY && priesecnik.y() < vacsieY)
					//ak je to naozaj priesecnik usecky a platna
					priesecniky++;
				//qDebug()<< priesecnik;
			}
			if (priesecniky > 0)
				trebakreslit = true;
			else
				trebakreslit = false;
			//qDebug() << trebakreslit;

		}


		if (trebakreslit) {

			for (i = 0; i < vrcholy.count(); i++) {
				w = body[0] - vrcholy[i];
				e = vrcholy[(i + 1) % 4] - vrcholy[i];
				n.setX(-e.y());
				n.setY(e.x());
				wn = w.dotProduct(n, w);
				dn = n.dotProduct(n, d);
				if (dn != 0) {
					t = -wn / dn;
					if (dn > 0 && t <= 1) {
						if (t > tl)
							tl = t;
					}
					else if (dn < 0 && t >= 0) {
						if (t < tu)
							tu = t;
						//qDebug() << tu << " " << t<< "\n";
					}
				}
			}
			if (tl < tu) {
				A = body[0] + (body[1] - body[0]) * tl;
				B = body[0] + (body[1] - body[0]) * tu;
				//qDebug() << tu << " " << tl << "\n";
				if (algo == 0)
					usecka_DDA(A, B, color);
				else
					usecka_Bresenham(A, B, color);
			}
		}
	}
		

	//polygon s orezanim
	QVector<QPoint> W, V;
	V = body;
		if (V.count() > 2) {
			//qDebug() << V;
			QPoint S, P, A, B;
			int Xmin[4] = { 0, 0, -img->width()+1, -img->height()+1 };
			int xmin = 0, j = 0, i=0;
			for (j = 0; j < 4; j++) {
				xmin = Xmin[j];
				if (V.count()>0)
					S = V[V.count() - 1];
				for (i = 0; i < V.count(); i++) {
					if (V[i].x() >= xmin) {
						if (S.x() >= xmin) {
							W.push_back(V[i]);
						}
						else {
							P.setX(xmin);
							P.setY(S.y() + (xmin - S.x()) * (V[i].y() - S.y()) / (V[i].x() - S.x()));
							W.push_back(P);
							W.push_back(V[i]);
						}
					}
					else {
						if (S.x() >= xmin) {
							P.setX(xmin);
							P.setY(S.y() + (xmin - S.x()) * (V[i].y() - S.y()) / (V[i].x() - S.x()));
							W.push_back(P);
						}
					}
					S = V[i];
				}
				//qDebug() << "W je: " << W;
				V.clear();
				for (i = 0; i < W.count(); i++) {
					P.setX(W[i].y());
					P.setY(-W[i].x());
					V.push_back(P);
				}
				W.clear();
			}
			for (i = 0; i < V.count(); i++) {
				A.setX(V[i].x());
				A.setY(V[i].y());
				B.setX(V[(i + 1) % (V.count())].x());
				B.setY(V[(i + 1) % (V.count())].y());
				if (algo == 0)
					usecka_DDA(A, B, color);
				else if (algo == 1)
					usecka_Bresenham(A, B, color);
			}
		
		}
	update();
}



	/*
	for (i = 1; i < body.count(); i++) {
		A.setX(body[i - 1].x());
		A.setY(body[i - 1].y());
		B.setX(body[i].x());
		B.setY(body[i].y());
		if (algo == 0)
			usecka_DDA(A, B, color);
		else if (algo == 1)
			usecka_Bresenham(A, B, color);
	}
	A.setX(body[0].x());
	A.setY(body[0].y());
	B.setX(body[body.count() - 1].x());
	B.setY(body[body.count() - 1].y());
	if (algo == 0)
		usecka_DDA(A, B, color);
	else if (algo == 1)
		usecka_Bresenham(A, B, color);

		*/


/*QVector<QPoint> ViewerWidget::rotacia(double uhol, QVector<QPoint> polygon) {
	QVector<QPoint> temp; 
	QPoint A;
	int x = polygon[0].x(), y = polygon[0].y(), i = 0, a, b;
	qDebug() << polygon <<"\n";
	if (uhol >= 0) {
		for (i = 0; i < polygon.count(); i++) {
			a = polygon[i].x() - x;
			b = polygon[i].y() - y;
			A.setX(a * qCos(-uhol * M_PI / 180) - b * qSin(-uhol * M_PI / 180) + x);
			A.setY(a * qSin(-uhol * M_PI / 180) + b * qCos(-uhol * M_PI / 180) + y);
			temp.push_back(A);
		}
	}
	if (uhol < 0) {
		for (i = 0; i < polygon.count(); i++) {
			a = polygon[i].x() - x;
			b = polygon[i].y() - y;
			A.setX(a * qCos(uhol * M_PI / 180) + b * qSin(uhol * M_PI / 180) + x);
			A.setY(-a * qSin(uhol * M_PI / 180) + b * qCos(uhol * M_PI / 180) + y);
			temp.push_back(A);
		}
	}
	return temp;
}

QVector<QPoint> ViewerWidget::skalovanie(double xf, double yf, QVector<QPoint> polygon) {
	QVector<QPoint> temp;
	QPoint A;
	int i;
	for (i = 0; i < polygon.count(); i++) {
		A.setX(polygon[0].x() + ((polygon[i].x() - polygon[0].x()) * xf));
		A.setY(polygon[0].y() + ((polygon[i].y() - polygon[0].y()) * yf));
		temp.push_back(A);
	}
	return temp;
}

QVector<QPoint> ViewerWidget::skosenie(double xf,  QVector<QPoint> polygon) {
	QVector<QPoint> temp;
	QPoint A;
	int i,a;
	for (i = 0; i < polygon.count(); i++) {
		a = xf * polygon[i].y();
		A.setX(polygon[i].x() + a);
		A.setY(polygon[i].y());
		temp.push_back(A);
		qDebug() << a;
	}
	qDebug() << temp << "\n" << polygon << "\n";
	return temp ;
}*/