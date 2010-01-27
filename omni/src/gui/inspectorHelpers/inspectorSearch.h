#ifndef INSPECTOR_SEARCH
#define INSPECTOR_SEARCH

	/*      
	   //   QHBoxLayout* setupSearchStuff();
	   void filterEnabledObjects();
	   void filterDisabledObjects();
	 */

/*
private slots:

	void filterEnabled(const QString & text );
	void filterSearch(const QString & text);
	void filterUnlocked();
	void filterLocked();
	*/

/*
QHBoxLayout* MyInspectorWidget::setupSearchStuff()
{

	QHBoxLayout *horizontalLayout = new QHBoxLayout();  

	// show all/diabled/enabled
	QComboBox   *filterComboBox   = new QComboBox(this);
	filterComboBox->addItem(QString("Show All"));
	filterComboBox->addItem(QString("Show Enabled"));
	filterComboBox->addItem(QString("Show Disabled"));
	horizontalLayout->addWidget(filterComboBox);

	QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	horizontalLayout->addItem(horizontalSpacer);
	
	// search box
	QLineEdit *searchEdit = new QLineEdit(this);
	horizontalLayout->addWidget(searchEdit);
	QComboBox *searchComboBox = new QComboBox(this);
	searchComboBox->addItem(QString("Search All"));
	searchComboBox->addItem(QString("Search Names"));
	searchComboBox->addItem(QString("Search IDs"));
	searchComboBox->addItem(QString("Search Tags"));
	searchComboBox->addItem(QString("Search Notes"));
	horizontalLayout->addWidget(searchComboBox);

	// A common use case is to let the user specify the filter regexp,
	//  wildcard pattern, or fixed string in a QLineEdit and to connect
	//  the textChanged() signal to setFilterRegExp(),
	
	connect( searchEdit, SIGNAL(textChanged(const QString &)), 
		  this, SLOT(filterSearch(const QString &)), Qt::DirectConnection);
	
	connect( filterComboBox, SIGNAL(currentIndexChanged( const QString &)), 
		 this, SLOT(filterEnabled(const QString &)), Qt::DirectConnection);

	return horizontalLayout;
}
*/

/*
void MyInspectorWidget::filterEnabled(const QString & text)
{
	if (text == "Show All") {
		filterUnlocked();
	}
	else if (text == "Show Enabled") {
		filterEnabledObjects();
	}
	else if (text == "Show Disabled") {
		filterDisabledObjects();
	}
	
	//		filterComboBox->addItem(QString("Show All"));
	//		filterComboBox->addItem(QString("Show Enabled"));
	//		filterComboBox->addItem(QString("Show Disabled"));
}

void MyInspectorWidget::filterUnlocked()
{
	proxyModel->setFilterRole(Qt::CheckStateRole);
	proxyModel->setFilterRegExp("");
	proxyModel->setFilterKeyColumn(0);
}

void MyInspectorWidget::filterLocked()
{
	//	int segment_id = segInspectorWidget->idSlider->sliderPosition();
	//	
	//	QVariant v_parent_id = proxyModel->data(view->currentIndex(), Qt::UserRole + 1);	// this will be the segmentation (parent) id
	//	OmId my_parent_id =  v_parent_id.value<OmId>();
	//	
	//	QVariant v_grandparent_id = proxyModel->data(view->currentIndex().parent(), Qt::UserRole + 1);
	//	OmId my_grandparent_id = v_grandparent_id.value<OmId>();
	//	
	//	// need to update checkbox
	//	QModelIndex my_index = view->currentIndex().child((segment_id - 1), 0);
	//	proxyModel->setData(my_index, Qt::Checked, Qt::CheckStateRole);
	//	view->update(my_index);
}

void MyInspectorWidget::filterEnabledObjects()
{
	//	proxyModel->invalidateFilter();
	proxyModel->setFilterRole(Qt::CheckStateRole);
	proxyModel->setFilterRegExp(QRegExp(QString::number(Qt::Checked), Qt::CaseInsensitive, QRegExp::FixedString));
	proxyModel->setFilterKeyColumn(0);
}

void MyInspectorWidget::filterDisabledObjects()
{
	//	proxyModel->invalidateFilter();
	proxyModel->setFilterRole(Qt::CheckStateRole);
	proxyModel->setFilterRegExp(QRegExp(QString::number(Qt::Unchecked), Qt::CaseInsensitive, QRegExp::FixedString));
	proxyModel->setFilterKeyColumn(0);
}

void MyInspectorWidget::filterSearch(const QString & text)
{
	//	proxyModel->invalidateFilter();
	proxyModel->setFilterRole(Qt::DisplayRole);
	proxyModel->setFilterRegExp(QRegExp(text));
	proxyModel->setFilterKeyColumn(0);
}
*/

#endif
