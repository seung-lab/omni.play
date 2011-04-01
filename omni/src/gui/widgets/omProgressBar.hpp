#ifndef OM_PROGRESS_BAR_HPP
#define OM_PROGRESS_BAR_HPP

template <typename T>
class OmProgressBar : public QGroupBox {
public:
    OmProgressBar(QWidget* parent)
        : QGroupBox(parent)
        , max_(0)
        , min_(0)
        , curPerc_(0)
    {
        progressBar_ = new QProgressBar(this);
        progressBar_->setTextVisible(false);
        progressBar_->setMinimum(0);
        progressBar_->setMaximum(100);

        label_ = new QLabel(this);

        overallContainer_ = new QHBoxLayout(this);
        overallContainer_->addWidget(progressBar_);
        overallContainer_->addWidget(label_);
    }

    void setMaximum(const T max){
        max_ = max;
    }

    void setMinimum(const T min){
        min_ = min;
    }

    void setValue(const T value)
    {
        if(!value || !max_){
            curPerc_ = 0;
        } else {
            curPerc_ = 100.0 * (value - min_) / max_;
        }

        progressBar_->setValue(curPerc_);

        const QString percentStr = QString("%1%").arg(curPerc_);
        label_->setText(percentStr);
    }

private:
    T max_;
    T min_;
    int curPerc_;

    QProgressBar* progressBar_;
    QLabel* label_;
    QHBoxLayout* overallContainer_;
};

#endif
