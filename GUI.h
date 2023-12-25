#pragma once

#include <QtWidgets/QListWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QCheckBox>
#include "SemanticSimilarity.h"

class GUI : public QWidget
{
public:
    explicit GUI(QWidget *parent = nullptr);
private:
    SemanticSimilarity semanticSimilarity;
    std::set<std::string> selectedWords;
    std::vector<QCheckBox*> checkboxes;
    QTextEdit *resultText;
    QListWidget *selectedWordsList;
};
