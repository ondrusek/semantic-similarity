#include <fstream>
#include <sstream>
#include <string>
#include <stack>
#include <QVBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QApplication>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLabel>
#include "GUI.h"

GUI::GUI(QWidget *parent) : QWidget (parent)
{
    auto *treeWidget = new QTreeWidget;
    treeWidget->setHeaderHidden(true);
    treeWidget->setSelectionMode(QAbstractItemView::NoSelection);

    std::stack<std::pair<QTreeWidgetItem*, std::string>> treeStack;

    std::ifstream dataFile("./data.txt");
    std::string lineString;
    std::string itemString;
    int depth;
    int nextDepth;
    std::vector<std::pair<int, std::string>> items;

    while (std::getline(dataFile, lineString)) {
        depth = 0;
        for (char c : lineString) {
            if (c == ' ') {
                ++depth;
            } else {
                break;
            }
        }
        itemString = lineString.substr(depth);
        depth /= 2;

        items.emplace_back(depth, itemString);
    }

    for (int i = 0; i < items.size(); ++i) {
        depth = items[i].first;
        itemString = items[i].second;

        if (i < items.size() - 1) {
            nextDepth = items[i+1].first;
        } else {
            nextDepth = 0;
        }

        QTreeWidgetItem *itemWidget;
        if (treeStack.empty()) {
            itemWidget = new QTreeWidgetItem(treeWidget);
        } else {
            itemWidget = new QTreeWidgetItem(treeStack.top().first);
        }

        if (depth > nextDepth) {
            for (int j = 0; j < depth - nextDepth; ++j) {
                if (!treeStack.empty()) {
                    treeStack.pop();
                }
            }
        }

        if (depth < nextDepth) {
            treeStack.push({itemWidget, itemString});
        }

        auto *treeLayout = new QBoxLayout(QBoxLayout::LeftToRight);
        treeLayout->setContentsMargins(5, 0, 0, 0);

        std::string subItemString;
        std::stringstream ss(itemString);
        while (std::getline(ss, subItemString, '/')) {
            auto *checkbox = new QCheckBox(QString::fromStdString(subItemString));
            checkboxes.push_back(checkbox);
            connect(checkbox, &QCheckBox::clicked, this, [checkbox, this]() {
                std::string selectedWord = checkbox->text().toStdString();
                if (checkbox->isChecked()) {
                    //if (!selectedWords.contains(selectedWord)) {
                    if (selectedWords.count(selectedWord) == 0) {
                        selectedWords.insert(selectedWord);
                    }
                } else {
                    selectedWords.erase(selectedWord);
                    for (const auto &checkboxIter : checkboxes) {
                        if (checkboxIter->text().toStdString() == selectedWord) {
                            checkboxIter->setCheckState(Qt::Unchecked);
                        }
                    }
                }
                selectedWordsList->clear();
                for (const auto &word : selectedWords) {
                    selectedWordsList->addItem(QString::fromStdString(word));
                }
                auto excludedWord = semanticSimilarity.getWordNotBelongingToSet(selectedWords);
                resultText->setText(QString::fromStdString(excludedWord));
            });
            treeLayout->addWidget(checkbox);
        }

        treeLayout->addStretch();

        auto *widget = new QWidget;
        widget->setLayout(treeLayout);

        treeWidget->setItemWidget(itemWidget, 0, widget);
    }

    selectedWordsList = new QListWidget;
    selectedWordsList->setSelectionMode(QAbstractItemView::NoSelection);
    connect(selectedWordsList, &QListWidget::doubleClicked, this, [this]() {
        std::string wordToRemove = selectedWordsList->currentItem()->text().toStdString();
        //if (selectedWords.contains(wordToRemove)) {
        if (selectedWords.count(wordToRemove) > 0) {
            selectedWords.erase(wordToRemove);
        }
        for (const auto& checkboxIter : checkboxes) {
            if (checkboxIter->text().toStdString() == wordToRemove) {
                checkboxIter->setCheckState(Qt::Unchecked);
            }
        }
        selectedWordsList->clear();
        for (const auto& word : selectedWords) {
            selectedWordsList->addItem(QString::fromStdString(word));
        }
        auto excludedWord = semanticSimilarity.getWordNotBelongingToSet(selectedWords);
        resultText->setText(QString::fromStdString(excludedWord));
    });

    resultText = new QTextEdit("");
    resultText->setReadOnly(true);
    resultText->setMaximumHeight(27);

    auto *vBoxRight = new QVBoxLayout;
    vBoxRight->addWidget(new QLabel("Vybraná slova"));
    vBoxRight->addWidget(selectedWordsList);
    vBoxRight->addSpacing(10);
    vBoxRight->addWidget(new QLabel("Do vybraných slov nepatří"));
    vBoxRight->addWidget(resultText);

    auto *vBoxLeft = new QVBoxLayout;
    vBoxLeft->addWidget(new QLabel("Všechna slova"));
    vBoxLeft->addWidget(treeWidget);

    auto *hBoxMain = new QHBoxLayout;
    hBoxMain->addLayout(vBoxLeft);
    hBoxMain->addLayout(vBoxRight);

    setLayout(hBoxMain);
}
