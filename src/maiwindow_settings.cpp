#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "textcodecselectiondialog.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>



void MainWindow::settingsLoad() {
  qDebug() << "MainWindow::settingsLoad()";
  // read settings
  m_defaultFont = this->font();
  if (m_settings.contains(settings_keys::fontKey)) {
    QString fontName = m_settings.value(settings_keys::fontKey).toString();
    m_defaultFont.fromString(fontName);
    qDebug() << "defaul font is " << fontName;
  }

  if (m_settings.contains(settings_keys::showToolbarKey)) {
    bool value = m_settings.value(settings_keys::showToolbarKey).toBool();
    ui->mainToolBar->setVisible(value);
    ui->actionshow_toolbar->setChecked(value);
  }

  m_default_text_encoding = defaultTextEncoding;
  if (m_settings.contains(settings_keys::defaultEncodingKey)) {
    auto value = m_settings.value(settings_keys::defaultEncodingKey).toString();
    m_default_text_encoding = value;
    qDebug() << "default encoding:" << m_default_text_encoding;
  }

  loadLayoutSettings();
  loadRecent();
  loadFiltersFromSettings();
}

void MainWindow::settingsSave() {
  qDebug() << "MainWindow::settingsSave()";
  saveFiltersSettings();
  saveLayoutSettings();
}

void MainWindow::saveHighlightFromSettings() {
  QJsonObject json;
  QJsonArray values;
  auto patterns = m_controller->m_global_highlight.values();
  for (auto v : patterns) {
    QJsonObject js_v;
    js_v["pattern"] = v.pattern;
    js_v["rgb"] = v.rgb;
    values.append(js_v);
  }
  json["patterns"] = values;
  QJsonDocument doc(json);
  QString strJson(doc.toJson(QJsonDocument::Compact));
  m_highlight_settings.setValue(settings_keys::highlightKey, strJson);
  qDebug() << ">>> " << strJson;
}

void MainWindow::loadHighlightFromSettings() {
  qDebug() << "MainWindow::loadHighlightFromSettings()";

  if (m_highlight_settings.contains(settings_keys::highlightKey)) {
    m_controller->m_global_highlight.clear();
    QString jsonStr = m_highlight_settings.value(settings_keys::highlightKey).toString();
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (!doc.isNull()) {
      if (doc.isObject()) {
        QJsonObject obj = doc.object();
        QJsonArray patterns = obj["patterns"].toArray();
        for (int i = 0; i < patterns.count(); ++i) {
          auto p = patterns.at(i).toObject();
          HighlightPattern hp;
          hp.pattern = p["pattern"].toString();
          hp.rgb = p["rgb"].toString();
          qDebug() << ">> " << hp.pattern << ":" << hp.rgb;
          m_controller->m_global_highlight.insert(hp.pattern, hp);
        }
      }
    } else {
      throw std::logic_error("highlights format error: " + jsonStr.toStdString());
    }
  } else {
    m_controller->m_global_highlight = default_highlight_settings;
  }
}

void MainWindow::saveFiltersSettings() {
  qDebug() << "MainWindow::saveFiltersSettings()";
  QJsonObject json;
  QJsonArray values;
  auto patterns = m_controller->m_global_highlight.values();
  for (auto v : _filter_form->m_filters) {
    QJsonObject js_v;
    js_v["pattern"] = v.pattern;
    js_v["enabled"] = v.is_enabled;
    values.append(js_v);
  }
  json["patterns"] = values;
  QJsonDocument doc(json);
  QString strJson(doc.toJson(QJsonDocument::Compact));
  m_filters_settings.setValue(settings_keys::filtersKey, strJson);
  qDebug() << ">>> " << strJson;
}

void MainWindow::loadFiltersFromSettings() {
  qDebug() << "MainWindow::loadFiltersFromSettings()";
  if (m_filters_settings.contains(settings_keys::filtersKey)) {
    QString jsonStr = m_filters_settings.value(settings_keys::filtersKey).toString();
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (!doc.isNull()) {
      if (doc.isObject()) {
        QJsonObject obj = doc.object();
        QJsonArray patterns = obj["patterns"].toArray();
        for (int i = 0; i < patterns.count(); ++i) {
          auto p = patterns.at(i).toObject();
          StringFilterDescription sfd;
          sfd.pattern = p["pattern"].toString();
          sfd.is_enabled = p["enabled"].toBool();
          qDebug() << ">> " << sfd.pattern << ":" << sfd.is_enabled;
          _filter_form->m_filters << sfd;
        }
        _filter_form->update();
      }
    } else {
      throw std::logic_error("filters format error: " + jsonStr.toStdString());
    }
  }
}

void MainWindow::saveLayoutSettings() {
  auto szs = ui->splitter->sizes();
  qDebug() << "save splitter sizes" << szs.size();
  m_settings.beginWriteArray(settings_keys::filterFrameWidthKey);
  int i = 0;
  for (auto s : szs) {
    m_settings.setArrayIndex(i++);
    m_settings.setValue("i", s);
  }
  m_settings.endArray();
}

void MainWindow::loadLayoutSettings() {
  if (m_settings.contains(settings_keys::filterFrameWidthKey)) {
    QList<int> szs;
    int len = m_settings.beginReadArray(settings_keys::filterFrameWidthKey);
    for (int i = 0; i < len; ++i) {
      m_settings.setArrayIndex(i);
      szs << m_settings.value("i").toInt();
    }
    m_settings.endArray();
    qDebug() << "read splitter sizes" << szs.size();
    ui->splitter->setSizes(szs);
  }
}

void MainWindow::settingsSaveFontSetting() {
  m_settings.setValue(settings_keys::fontKey, m_defaultFont.toString());
  qDebug() << "selected font: " << m_defaultFont.toString();
}
