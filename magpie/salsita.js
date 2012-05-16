var apiImpl = _salsita_impl;

function getParam(obj, name, defVal)
{
  if (name in obj) {
    return obj[name];
  } else {
    return defVal;
  }
}

function createTab()
{
  return { id: apiImpl.currentTabId };
}

function createSender()
{
  var sender = {};
  sender.tab = createTab();
  if (sender.tab.id == -1)
  {
    delete sender.tab;
  }
  sender.id = apiImpl.extensionId;
  return sender;
}

salsita.extension = {};
salsita.extension.sendRequest = function (request, callback) {
  apiImpl.performSendRequest(-1, createSender(), request, callback);
};
salsita.extension.onRequest = {};
salsita.extension.onRequest.addListener = function (listener) {
  apiImpl.addEventListener('extension.onRequest', listener);
};
salsita.extension.getURL = function (url) {
  return apiImpl.getResourceUrl(url);
};

salsita.tabs = {};
salsita.tabs.getCurrent = function (callback) {
  var t = createTab();
  if (t.id == -1)
  {
    callback();
  } else {
    callback(t);
  }
};
salsita.tabs.sendRequest = function (tabId, request, callback) {
  apiImpl.performSendRequest(tabId, createSender(), request, callback);
};
salsita.tabs.onCreated = {};
salsita.tabs.onCreated.addListener = function (listener) { // TODO: implement in the c++ code
  apiImpl.addEventListener('tabs.onCreated', function (_tabId) { listener({ tabId: _tabId }); });
};
salsita.tabs.onActivated = {};
salsita.tabs.onActivated.addListener = function (listener) {
  apiImpl.addEventListener('tabs.onActivated', function (_tabId) { listener({ tabId: _tabId }); });
};
salsita.tabs.onRemoved = {};
salsita.tabs.onRemoved.addListener = function (listener) { // TODO: implement in the c++ code
  apiImpl.addEventListener('tabs.onRemoved', listener);
};

// content script api

if (typeof _salsita_content_impl !== "undefined") {
  var contentApiImpl = _salsita_content_impl;

  salsita.content = {};

  salsita.content.openNewTab = function (p) {
    return contentApiImpl.openNewTab(p.url, getParam(p, 'active', true));
  };

  salsita.content.navigateTo = function (p) {
    return contentApiImpl.navigateTo(p.url);
  };

  salsita.content.openPopupWindow = function (p, callback) {
    return contentApiImpl.openPopupWindow(
      getParam(p, 'url', null),
      getParam(p, 'left', -1),
      getParam(p, 'top', -1),
      getParam(p, 'width', -1),
      getParam(p, 'height', -1),
      function(id, wnd) { callback({id: id, window: wnd }); });
  };

  salsita.content.closePopupWindow = function(id) {
    return contentApiImpl.closePopupWindow(id);
  };

  salsita.content.closeAllPopupWindows = function() {
    return contentApiImpl.closeAllPopupWindows();
  };

  // menu api
  // p has parent, rect
  // rect has left, top, width, height; left and top are relative to parent's topleft corner
  // position properties used here work in ie but might not work elsewhere!
  salsita.content.openMenu = function (p, callback) {
    var el = p.parent;
    var top = el.offsetTop - el.scrollTop;
    var left = el.offsetLeft - el.scrollLeft;

    while (el.offsetParent && el.offsetParent !== el) {
      el = el.offsetParent;
      top += el.offsetTop - el.scrollTop;
      left += el.offsetLeft - el.scrollLeft;
    };

    if (el.parentElement) {
      top -= el.parentElement.scrollTop;
      left -= el.parentElement.scrollLeft;
    }

    top += el.ownerDocument.parentWindow.screenTop;
    left += el.ownerDocument.parentWindow.screenLeft;

    var w = -1, h = -1;
    if ('rect' in p) {
      top += getParam(p.rect, 'top', 0);
      left += getParam(p.rect, 'left', 0);
      w = getParam(p.rect, 'width', 0);
      h = getParam(p.rect, 'height', 0);
    }

    (function (_wndPar) {
      var cbFunc = function (wnd) {
        callback(
          {
            menu: { identity: wnd.id, rect: _wndPar },
            root: wnd.window.document.body,
            window: wnd.window
          });
      };
      salsita.content.openPopupWindow(_wndPar, cbFunc);
    })({ left: left, top: top, width: w, height: h });
  };

  salsita.content.closeMenu = function (identity) {
    salsita.content.closePopupWindow(identity);
  };
}
