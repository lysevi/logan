#include "mainwindow.h"
#include "textcodecselectiondialog.h"
#include "ui_mainwindow.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

Settings::Settings(MainWindow *window)
    : m_settings("lysevi", "logan"), m_highlight_settings("lysevi", "logan_highlights"),
      m_recent_files_settings("lysevi", "logan_highlights"),
      m_filters_settings("lysevi", "logan_filters") {
  _mainWindow = window;
}

void Settings::load() {
  qDebug() << "Settings::settingsLoad()";
  // read settings
  loadHighlight();
  _mainWindow->m_defaultFont = _mainWindow->font();
  if (m_settings.contains(settings_keys::fontKey)) {
    QString fontName = m_settings.value(settings_keys::fontKey).toString();
    _mainWindow->m_defaultFont.fromString(fontName);
    qDebug() << "defaul font is " << fontName;
  }

  if (m_settings.contains(settings_keys::showToolbarKey)) {
    bool value = m_settings.value(settings_keys::showToolbarKey).toBool();
    _mainWindow->ui->mainToolBar->setVisible(value);
    _mainWindow->ui->actionshow_toolbar->setChecked(value);
  }

  _mainWindow->m_default_text_encoding = defaultTextEncoding;
  if (m_settings.contains(settings_keys::defaultEncodingKey)) {
    auto value = m_settings.value(settings_keys::defaultEncodingKey).toString();
    _mainWindow->m_default_text_encoding = value;
    qDebug() << "default encoding:" << _mainWindow->m_default_text_encoding;
  }

  loadLayout();
  loadRecent();
  loadFilters();
}

void Settings::save() {
  qDebug() << "Settings::settingsSave()";
  saveFilters();
  saveLayout();
  saveRecent();
}

void Settings::saveHighlight() {
  QJsonObject json;
  QJsonArray values;
  auto patterns = _mainWindow->m_controller->m_global_highlight.values();
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

void Settings::loadHighlight() {
  qDebug() << "Settings::loadHighlightFromSettings()";

  if (m_highlight_settings.contains(settings_keys::highlightKey)) {
    _mainWindow->m_controller->m_global_highlight.clear();
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
          _mainWindow->m_controller->m_global_highlight.insert(hp.pattern, hp);
        }
      }
    } else {
      throw std::logic_error("highlights format error: " + jsonStr.toStdString());
    }
  } else {
    _mainWindow->m_controller->m_global_highlight = default_highlight_settings;
  }
}

void Settings::saveFilters() {
  qDebug() << "Settings::saveFiltersSettings()";
  QJsonObject json;
  QJsonArray values;
  auto patterns = _mainWindow->m_controller->m_global_highlight.values();
  for (auto v : _mainWindow->_filter_form->m_filters) {
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

void Settings::loadFilters() {
  qDebug() << "Settings::loadFiltersFromSettings()";
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
          _mainWindow->_filter_form->m_filters << sfd;
        }
        _mainWindow->_filter_form->update();
      }
    } else {
      throw std::logic_error("filters format error: " + jsonStr.toStdString());
    }
  }
}

void Settings::saveLayout() {
  auto szs = _mainWindow->ui->splitter->sizes();
  qDebug() << "save splitter sizes" << szs.size();
  m_settings.beginWriteArray(settings_keys::filterFrameWidthKey);
  int i = 0;
  for (auto s : szs) {
    m_settings.setArrayIndex(i++);
    m_settings.setValue("i", s);
  }
  m_settings.endArray();
}

void Settings::loadLayout() {
  if (m_settings.contains(settings_keys::filterFrameWidthKey)) {
    QList<int> szs;
    int len = m_settings.beginReadArray(settings_keys::filterFrameWidthKey);
    for (int i = 0; i < len; ++i) {
      m_settings.setArrayIndex(i);
      szs << m_settings.value("i").toInt();
    }
    m_settings.endArray();
    qDebug() << "read splitter sizes" << szs.size();
    _mainWindow->ui->splitter->setSizes(szs);
  }
}

void Settings::saveFont() {
  m_settings.setValue(settings_keys::fontKey, _mainWindow->m_defaultFont.toString());
  qDebug() << "selected font: " << _mainWindow->m_defaultFont.toString();
}

void Settings::saveRecent() {
  qDebug() << "Settings::saveRecent()";
  if (_mainWindow->_recent_files.size() > 0) {
    m_recent_files_settings.beginWriteArray(settings_keys::recentFilesKey);
    for (int i = 0; i < _mainWindow->_recent_files.size(); ++i) {
      m_recent_files_settings.setArrayIndex(i);
      auto fname = _mainWindow->_recent_files[i];
      qDebug() << fname;
      m_recent_files_settings.setValue("file", fname);
    }
    m_recent_files_settings.endArray();
  }
}

void Settings::loadRecent() {
  qDebug() << "Settings::loadRecent()";
  int sz = m_recent_files_settings.beginReadArray(settings_keys::recentFilesKey);
  for (int i = 0; i < sz; ++i) {
    m_recent_files_settings.setArrayIndex(i);
    auto fname = m_recent_files_settings.value("file").toString();
    QFileInfo check_file(fname);
    qDebug() << fname;
    if (check_file.exists() && check_file.isFile()) {
      _mainWindow->_recent_files.append(fname);
    } else {
      qDebug() << fname << "not exists or not a file";
    }
  }
  m_recent_files_settings.endArray();
  _mainWindow->updateRecentFileMenu();
  saveRecent();
}
