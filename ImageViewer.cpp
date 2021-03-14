#include "ImageViewer.h"

ImageViewer::ImageViewer(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::ImageViewerClass)
{
	ui->setupUi(this);
	openNewTabForImg(new ViewerWidget("default", QSize(500, 500)));
	ui->farba->setStyleSheet("background-color: black");
	ui->transform->setVisible(false);
}

//ViewerWidget functions
ViewerWidget* ImageViewer::getViewerWidget(int tabId)
{
	QScrollArea* s = static_cast<QScrollArea*>(ui->tabWidget->widget(tabId));
	if (s) {
		ViewerWidget* vW = static_cast<ViewerWidget*>(s->widget());
		return vW;
	}
	return nullptr;
}
ViewerWidget* ImageViewer::getCurrentViewerWidget()
{
	return getViewerWidget(ui->tabWidget->currentIndex());
}

// Event filters
bool ImageViewer::eventFilter(QObject* obj, QEvent* event)
{
	if (obj->objectName() == "ViewerWidget") {
		return ViewerWidgetEventFilter(obj, event);
	}
	return false;
}

//ViewerWidget Events
bool ImageViewer::ViewerWidgetEventFilter(QObject* obj, QEvent* event)
{
	ViewerWidget* w = static_cast<ViewerWidget*>(obj);

	if (!w) {
		return false;
	}

	if (event->type() == QEvent::MouseButtonPress) {
		ViewerWidgetMouseButtonPress(w, event);
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		ViewerWidgetMouseButtonRelease(w, event);
	}
	else if (event->type() == QEvent::MouseMove) {
		ViewerWidgetMouseMove(w, event);
	}
	else if (event->type() == QEvent::Leave) {
		ViewerWidgetLeave(w, event);
	}
	else if (event->type() == QEvent::Enter) {
		ViewerWidgetEnter(w, event);
	}
	else if (event->type() == QEvent::Wheel) {
		ViewerWidgetWheel(w, event);
	}

	return QObject::eventFilter(obj, event);
}
void ImageViewer::ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (!nakreslene) {
		if (e->button() == Qt::LeftButton) {
			if (!ui->polygon->isChecked()) {
				if (prvybod) {
					B = e->pos();
					if (ui->comboBox->currentIndex() == 0) {
						getCurrentViewerWidget()->usecka_DDA(A, B, farba);
					}
					else if (ui->comboBox->currentIndex() == 1) {
						getCurrentViewerWidget()->usecka_Bresenham(A, B, farba);
					}
					prvybod = false;
					nakreslene = true;
					poly.push_back(A);
					poly.push_back(B);
					ui->transform->setVisible(true);
					//getCurrentViewerWidget()->kresliPolygon(poly, farba, 0);
					/*else if (ui->kruznica->isChecked()) {
						getCurrentViewerWidget()->kruznica(A, B, farba);
						prvybod = false;
					}
					else {
						msgBox.setText(u8"Pre vykreslenie zvo¾te algoritmus.");
						msgBox.setIcon(QMessageBox::Information);
						msgBox.exec();
						prvybod = false;

					}*/
				}
				else {
					A = e->pos();
					prvybod = true;
					ui->comboBox->setEnabled(false);
					ui->polygon->setEnabled(false);
				}
			}
			else if (ui->polygon->isChecked()) {
				if (!prvybod) {
					A = e->pos();
					poly.push_back(A);
					prvybod = true;
					ui->comboBox->setEnabled(false);
					ui->polygon->setEnabled(false);
				}
				else {
					B = e->pos();
					poly.push_back(B);
					if (ui->comboBox->currentIndex() == 0) {
						getCurrentViewerWidget()->usecka_DDA(A, B, farba);
					}
					else if (ui->comboBox->currentIndex() == 1) {
						getCurrentViewerWidget()->usecka_Bresenham(A, B, farba);
					}
					A.setX(B.x());
					A.setY(B.y());
				}
			}
		}
		if (e->button() == Qt::RightButton && ui->polygon->isChecked() && prvybod) {
			B = poly[poly.count() - 1];
			A = poly[0];
			if (ui->comboBox->currentIndex() == 0) {
				getCurrentViewerWidget()->usecka_DDA(A, B, farba);
			}
			else if (ui->comboBox->currentIndex() == 1) {
				getCurrentViewerWidget()->usecka_Bresenham(A, B, farba);
			}
			//getCurrentViewerWidget()->kresliPolygon(poly, farba, ui->comboBox->currentIndex());
			nakreslene = true;
			ui->transform->setVisible(true);
		}
	}
	else {
		A = e->pos();
	}
		
}

void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (e->button() == Qt::LeftButton && nakreslene && !temp.isEmpty()) {
		poly = temp;
		temp.clear();
	}
}

void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);

	if (e->buttons() == Qt::LeftButton && nakreslene) {
		B = e->pos();
		temp.clear();
		QPoint C;
		//qDebug() << B << A << "\n";
		int dx = B.x() - A.x(), dy = B.y() - A.y(), i=0;
		//qDebug() << dx << "\n" << dy << "\n" ;
		for (i = 0; i < poly.count(); i++) {
			C.setX(poly[i].x() + dx);
			C.setY(poly[i].y() + dy);
			temp.push_back(C);
		}
		getCurrentViewerWidget()->kresliPolygon(temp, farba, ui->comboBox->currentIndex());
	}
}

void ImageViewer::ViewerWidgetLeave(ViewerWidget* w, QEvent* event)
{
}

void ImageViewer::ViewerWidgetEnter(ViewerWidget* w, QEvent* event)
{
}

void ImageViewer::ViewerWidgetWheel(ViewerWidget* w, QEvent* event)
{
	QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
	if (nakreslene) {
		int i;
		double coef;
		if (wheelEvent->angleDelta().y() > 0) 
			coef = 1.25;
		else 
			coef = 0.75;
		int x = poly[0].x(), y = poly[0].y();
		for (i = 0; i < poly.count(); i++) {
			poly[i].setX(x + ((poly[i].x() - x) * coef));
			poly[i].setY(y + ((poly[i].y() - y) * coef));
		}
		getCurrentViewerWidget()->kresliPolygon(poly, farba, ui->comboBox->currentIndex());	
	}
}

//ImageViewer Events
void ImageViewer::closeEvent(QCloseEvent* event)
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
	{
		event->accept();
	}
	else {
		event->ignore();
	}
}

//Image functions
void ImageViewer::openNewTabForImg(ViewerWidget* vW)
{
	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setWidget(vW);

	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	vW->setObjectName("ViewerWidget");
	vW->installEventFilter(this);

	QString name = vW->getName();

	ui->tabWidget->addTab(scrollArea, name);
}
bool ImageViewer::openImage(QString filename)
{
	QFileInfo fi(filename);

	QString name = fi.baseName();
	openNewTabForImg(new ViewerWidget(name, QSize(0, 0)));
	ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);

	ViewerWidget* w = getCurrentViewerWidget();

	QImage loadedImg(filename);
	return w->setImage(loadedImg);
}
bool ImageViewer::saveImage(QString filename)
{
	QFileInfo fi(filename);
	QString extension = fi.completeSuffix();
	ViewerWidget* w = getCurrentViewerWidget();

	QImage* img = w->getImage();
	return img->save(filename, extension.toStdString().c_str());
}
void ImageViewer::clearImage()
{
	ViewerWidget* w = getCurrentViewerWidget();
	w->clear();
}
void ImageViewer::setBackgroundColor(QColor color)
{
	if (!isImgOpened()) {
		msgBox.setText("No image is opened.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	ViewerWidget* w = getCurrentViewerWidget();
	w->clear(color);
}

//Slots

//Tabs slots
void ImageViewer::on_tabWidget_tabCloseRequested(int tabId)
{
	ViewerWidget* vW = getViewerWidget(tabId);
	delete vW; //vW->~ViewerWidget();
	ui->tabWidget->removeTab(tabId);
}
void ImageViewer::on_actionRename_triggered()
{
	if (!isImgOpened()) {
		msgBox.setText("No image is opened.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	ViewerWidget* w = getCurrentViewerWidget();
	bool ok;
	QString text = QInputDialog::getText(this, QString("Rename image"), tr("Image name:"), QLineEdit::Normal, w->getName(), &ok);
	if (ok && !text.trimmed().isEmpty())
	{
		w->setName(text);
		ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), text);
	}
}

//Image slots
void ImageViewer::on_actionNew_triggered()
{
	newImgDialog = new NewImageDialog(this);
	connect(newImgDialog, SIGNAL(accepted()), this, SLOT(newImageAccepted()));
	newImgDialog->exec();
}
void ImageViewer::newImageAccepted()
{
	NewImageDialog* newImgDialog = static_cast<NewImageDialog*>(sender());

	int width = newImgDialog->getWidth();
	int height = newImgDialog->getHeight();
	QString name = newImgDialog->getName();
	openNewTabForImg(new ViewerWidget(name, QSize(width, height)));
	ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
}
void ImageViewer::on_actionOpen_triggered()
{
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load image", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

	if (!openImage(fileName)) {
		msgBox.setText("Unable to open image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}
void ImageViewer::on_actionSave_as_triggered()
{
	if (!isImgOpened()) {
		msgBox.setText("No image to save.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	QString folder = settings.value("folder_img_save_path", "").toString();

	ViewerWidget* w = getCurrentViewerWidget();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder + "/" + w->getName(), fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

	if (!saveImage(fileName)) {
		msgBox.setText("Unable to save image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
	else {
		msgBox.setText(QString("File %1 saved.").arg(fileName));
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
	}
}
void ImageViewer::on_actionClear_triggered()
{
	if (!isImgOpened()) {
		msgBox.setText("No image is opened.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	clearImage();
}
void ImageViewer::on_actionSet_background_color_triggered()
{
	QColor backgroundColor = QColorDialog::getColor(Qt::white, this, "Select color of background");
	if (backgroundColor.isValid()) {
		setBackgroundColor(backgroundColor);
	}
}

void ImageViewer::on_color_clicked()
{
	QColor Color = QColorDialog::getColor(Qt::white, this, "Select color");
	if (Color.isValid()) {
		farba = Color;
		QString qss = QString("background-color: %1 ").arg(Color.name());
		ui->farba->setStyleSheet(qss);
	}
	if (!poly.isEmpty())
		getCurrentViewerWidget()->kresliPolygon(poly, farba, ui->comboBox->currentIndex());
}

void ImageViewer::on_Clear_clicked() {
	nakreslene = false;
	prvybod = false;
	poly.clear();
	temp.clear();
	ui->polygon->setEnabled(true);
	ui->comboBox->setEnabled(true);
	ui->transform->setVisible(false);
	clearImage();
	update();
}

void ImageViewer::on_rot_clicked() {
	double uhol = ui->rot_spin->value();
	int x = poly[0].x(), y = poly[0].y(), i = 0, a, b;
	//qDebug() << poly << "\n";
	if (uhol >= 0) {
		for (i = 0; i < poly.count(); i++) {
			a = poly[i].x() - x;
			b = poly[i].y() - y;
			poly[i].setX(a * qCos(-uhol * M_PI / 180) - b * qSin(-uhol * M_PI / 180) + x);
			poly[i].setY(a * qSin(-uhol * M_PI / 180) + b * qCos(-uhol * M_PI / 180) + y);
		}
	}
	if (uhol < 0) {
		for (i = 0; i < poly.count(); i++) {
			a = poly[i].x() - x;
			b = poly[i].y() - y;
			poly[i].setX(a * qCos(uhol * M_PI / 180) + b * qSin(uhol * M_PI / 180) + x);
			poly[i].setY(-a * qSin(uhol * M_PI / 180) + b * qCos(uhol * M_PI / 180) + y);
		}
	}
	getCurrentViewerWidget()->kresliPolygon(poly, farba, ui->comboBox->currentIndex());
}

void ImageViewer::on_sca_clicked() {
	int i;
	for (i = 0; i < poly.count(); i++) {
		poly[i].setX(poly[0].x() + ((poly[i].x() - poly[0].x()) * ui->sca_x->value()));
		poly[i].setY(poly[0].y() + ((poly[i].y() - poly[0].y()) * ui->sca_y->value()));
	}
	getCurrentViewerWidget()->kresliPolygon(poly, farba, ui->comboBox->currentIndex());
}

void ImageViewer::on_sko_clicked() {
	int i, a;
	for (i = 0; i < poly.count(); i++) {
		a = ui->sko_spin->value() * (poly[i].y()-poly[0].y());
		poly[i].setX(poly[i].x() + a);
		poly[i].setY(poly[i].y());
		//qDebug() << a;
	}
	getCurrentViewerWidget()->kresliPolygon(poly, farba, ui->comboBox->currentIndex());
}

void ImageViewer::on_sym_clicked() {
	if (ui->polygon->isChecked()) {				//os sumernosti je urcena prvym a druhym bodom polygonu
		int i;
		double a = poly[1].y() - poly[0].y();
		double b = -(poly[1].x() - poly[0].x());
		double c = -a * poly[0].x() - b * poly[0].y();

		for (i = 2; i < poly.count(); i++) {
			double x = poly[i].x();
			double y = poly[i].y();
			poly[i].setX(x - 2 * a * (a * x + b * y + c) / (a * a + b * b));
			poly[i].setY(y - 2 * b * (a * x + b * y + c) / (a * a + b * b));
		}
	}
	else {									//vertikalna os cez prvy bod
		poly[1].setX(poly[0].x() - poly[1].x() + poly[0].x());
	}
	getCurrentViewerWidget()->kresliPolygon(poly, farba, ui->comboBox->currentIndex());
}

