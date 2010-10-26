#include "gui/segmentLists/elementListBox.hpp"
#include "gui/segmentLists/elementListBoxImpl.hpp"

void ElementListBox::Create(OmViewGroupState* vgs){
	instance().impl_ = boost::make_shared<ElementListBoxImpl>(vgs);
}

void ElementListBox::Reset() {
	instance().impl_->reset();
}

void ElementListBox::SetActiveTab(QWidget* tab){
	instance().impl_->setActiveTab(tab);
}

void ElementListBox::AddTab(const int preferredIndex, QWidget* tab,
							const QString& tabTitle){
	instance().impl_->addTab(preferredIndex, tab, tabTitle);
}

QWidget* ElementListBox::Widget() {
	return instance().impl_.get();
}

void ElementListBox::SetTitle(const QString& title){
	instance().impl_->setTitle(title);
}

void ElementListBox::Delete(){
	instance().impl_.reset();
}

void ElementListBox::PopulateLists(){
	instance().impl_->PopulateLists();
}

void ElementListBox::RebuildLists(const SegmentDataWrapper& sdw){
	instance().impl_->RebuildLists(sdw);
}

void ElementListBox::UpdateSegmentListBox(const SegmentationDataWrapper& sdw){
	instance().impl_->UpdateSegmentListBox(sdw);
}
