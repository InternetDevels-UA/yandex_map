Drupal.yandex_maps = Drupal.yandex_maps || {};
Drupal.yandex_maps.maps = Drupal.yandex_maps.maps || {};

(function($) {
  /**
   * Core behavior for Yandex Map.
   */
  Drupal.behaviors.yandex_map = {
    attach: function (context, settings) {
      if (Drupal.settings.yandex_map.maps) {
        var admin_id = 'admin_preview';
        if (Drupal.settings.yandex_map.maps[admin_id]) {
          // Handle right/left cursors in position fields.
          $("input[type=text][name^=yandex_map_controls_]", context)
            .keydown(function(e){
              if (parseInt($(this).val()) != $(this).val() || parseInt($(this).val()) < 1) {
                $(this).val('');
              }
              if (e.keyCode == 40)
                $(this).val(parseInt($(this).val()) - 1);
              if (e.keyCode == 38)
                $(this).val(parseInt($(this).val()) + 1);
              $(this).change();
            });

          // Handle enable/disable checkboxes.
          $("input[type=checkbox][name^=yandex_map_controls_]", context).change(function() {
            var type = $(this).attr('name').split('_')[3];
            Drupal.settings.yandex_map.maps[admin_id].controls[type].enable = $(this).attr('checked');
            Drupal.yandex_maps.maps[admin_id].rebuild();
          });

          // Handle changes of position textfields..
          $("input[type=text][name^=yandex_map_controls_]", context).change(function(){
            var type = $(this).attr('name').split('_')[3];
            var pos = $(this).attr('name').split('_')[4];
            Drupal.settings.yandex_map.maps[admin_id].controls[type][pos] = $(this).val();
            Drupal.yandex_maps.maps[admin_id].rebuild();
          });

          ymaps.ready(function(){
            // Handle changes of map center, zoom and type.
            Drupal.yandex_maps.maps[admin_id].object.events.add(['boundschange', 'typechange'], function() {
              var map = Drupal.yandex_maps.maps[admin_id].object;

              var zoom = map.getZoom();
              Drupal.settings.yandex_map.maps[admin_id].zoom = zoom;
              jQuery("input[name=yandex_map_default_zoom]").val(zoom);
              jQuery("#" + admin_id + "_zoom").html(Drupal.t("Zoom: !value", {'!value': zoom}));

              var center = map.getCenter().toString();
              Drupal.settings.yandex_map.maps[admin_id].center = center;
              jQuery("input[name=yandex_map_default_center]").val(center);
              jQuery("#" + admin_id + "_center").html(Drupal.t("Map center: !value", {'!value': center}));

              var type = map.getType().split('#')[1];
              jQuery("select[name=yandex_map_default_type]").val(type);
              Drupal.settings.yandex_map.maps[admin_id].type = type;
            });
          });
        }
      }
    }
  }
})(jQuery);

ymaps.ready(function(){
  if (Drupal.settings.yandex_map.maps) {
    jQuery.each(Drupal.settings.yandex_map.maps, function(id, settings) {
      // Create map and save object to Drupal storage.
      Drupal.yandex_maps.maps[id] = yandex_map(settings);
    });
  }
});

function yandex_map(settings) {
  var map = this;

  // Prepare defaults.
  map.colors = {
    'blue':       { title: Drupal.t('Blue'),        hex: '#006CFF' },
    'darkblue':   { title: Drupal.t('Dark blue'),   hex: '#00339A' },
    'darkgreen':  { title: Drupal.t('Dark green'),  hex: '#004056' },
    'darkorange': { title: Drupal.t('Dark orange'), hex: '#FF6600' },
    'green':      { title: Drupal.t('Green'),       hex: '#33CC00' },
    'grey':       { title: Drupal.t('Grey'),        hex: '#C2CCD2' },
    'lightblue':  { title: Drupal.t('Light blue'),  hex: '#66C7FF' },
    'orange':     { title: Drupal.t('Orange'),      hex: '#FFB400' },
    'pink':       { title: Drupal.t('Pink'),        hex: '#B832FD' },
    'red':        { title: Drupal.t('Red'),         hex: '#FF0000' },
    'violet':     { title: Drupal.t('Violet'),      hex: '#FD32FB' },
    'white':      { title: Drupal.t('White'),       hex: '#FFFFFF' },
    'yellow':     { title: Drupal.t('Yellow'),      hex: '#FFEA00' },
    'brown':      { title: Drupal.t('Brown'),       hex: '#39230E' },
    'black':      { title: Drupal.t('Black'),       hex: '#000000' }
  };

  map.geoobject_default_properties = {
    iconContent: ''
  };

  map.geoobject_default_options = {
    draggable: false,
    hideIconOnBallon: false,
    balloonCloseButton: true,

    strokeColor: '#FF0000',
    strokeStyle: 'solid',
    strokeWidth: 5,

    fillColor: '#FF0000',
    fillOpacity: 0.5,

    preset: "twirl#redDotIcon"
  };

  if (settings.mode == 'edit') {
    map.geoobject_default_options.draggable = true;
    map.geoobject_default_properties.hintContent = Drupal.t("Click to edit/delete or drag to move");
  }

  // Configure map id.
  map.id = settings['map_id'];

  // Rebuild all map with new params.
  map.rebuild = function() {
    if (map.object == null) return;
    map.object.destroy();
    map.object = null;
    yandex_map(Drupal.settings.yandex_map.maps[map.id]);
  }

  // Create map.
  map.object = new ymaps.Map(map.id, {
    center: settings['center'].split(','),
    zoom: settings['zoom'],
    type: 'yandex#' + settings['type']
  });
  map.object.id = map.id;

  // Reset controls and markers.
  map.controls = {};
  map.buttons = ['placemark', 'line', 'polygon', 'circle', 'rectangle'];
  map.content = new ymaps.GeoObjectCollection();

  // Handle behaviors status.
  jQuery.each(settings['behaviors'], function(k, v){
    if (k == v) {
      if (map.object.behaviors.isEnabled(k) == false) {
        map.object.behaviors.enable(k);
      }
    }
    else if (map.object.behaviors.isEnabled(k) == true) {
      map.object.behaviors.disable(k);
    }
  });

  // Helper function for top/bottom and right/left params.
  map.control_param = function(settings) {
    var options = {};

    if (settings.top) {
      options.top = settings.top;
    }
    else if (settings.bottom) {
      options.bottom = settings.bottom;
    }

    if (settings.left) {
      options.left = settings.left;
    }
    else if (settings.right) {
      options.right = settings.right;
    }

    if (settings.width) {
      //options.width = parseInt(settings.width);
    }

    return options;
  }

  // Add map controls.
  jQuery.each(settings.controls, function(type, control) {
    if (control) {
      if (type == 'editTools' && settings.editTools && settings.mode == 'edit') {
        // Create edit tools toolbar.
        var tools = [];
        map.editTools = {};
        map.editTools.tools = [];
        map.editTools.object = new ymaps.control.ToolBar([]);
        jQuery.each(map.buttons, function(i, type) {
          if (settings.editTools[type]) {
            map.editTools.tools[type] = new ymaps.control.Button({data: {
              image: Drupal.settings.basePath + Drupal.settings.yandex_map.path + '/images/' + type + '.png'
            }});
            map.editTools.tools[type].id = type;
            map.editTools.object.add(map.editTools.tools[type]);
            map.editTools.tools[type].events.add('select', function(e) {
              map.editTools.object.each(function(tool){
                if (tool.id != e.originalEvent.target.id) {
                  tool.deselect();
                }
              });
            });
          }
        });
        type = map.editTools.object;
      }

      if (type != 'editTools' && control.enable == true) {
        var options = map.control_param(control);
        if (Object.keys(options).length) {
          map.controls[type] = map.object.controls.add(type, options);
        }
        else {
          map.controls[type] = map.object.controls.add(type);
        }
      }
    }
  });

  // Update maps type selectors.
  if (settings.controls.typeSelector.enable == true) {
    var types = [];
    jQuery.each(settings['types'], function(k, v) {
      if (k == v) {
        types.push('yandex#' + k);
      }
    });
    if (types.length) {
      map.object.controls.add(new ymaps.control.TypeSelector(types));
    }
  }

  map.baloon_template = function(object) {
    // Set default data.
    var object_id = object.properties.get('drupal.object_id', '');
    var map_id = object.properties.get('drupal.map_id', '');
    var type = object.properties.get('drupal.type', '');
    var desc = object.properties.get('drupal.description', '');
    var icon = object.properties.get('drupal.icon', '');
    var color = object.properties.get('drupal.color', 'red');
    var opacity = object.properties.get('drupal.opacity', 50);
    var thickness = object.properties.get('drupal.thickness', 5);

    // Prepare balloon form.
    var wrapper = jQuery('<div class="ymap_balloon" id="ymap_balloon_' + map_id + '_' + + object_id + '"></div>');

    // Prepare description textarea.
    var edit_desc = '<div class="desc">\
      <label class="label">' + Drupal.t('Description') + ':</label>\
      <label class="hint" style="display:' + (desc ? 'none' : 'block') + '">' + Drupal.t("Short description") + '</label>\
      <textarea onfocus="Drupal.yandex_maps.actions.description(this, true)" onblur="Drupal.yandex_maps.actions.description(this, false)">'+(desc || '')+'</textarea>\
    </div>';
    wrapper.append(edit_desc);

    // Prepare icon text fiedld.
    if (type == 'placemark' || type == 'circle') {
      var edit_icon = '<div class="title">\
        <label class="label">' + Drupal.t(type == 'placemark' ? 'Text on marker' : 'Radius (meters)') + ':</label>\
        <input type="text" value="' + (icon || '') + '" maxlength="16" />\
      </div>';
      wrapper.append(edit_icon);
    }

    // Prepare color table.
    var edit_color =
      '<table class="color">\
        <tbody>\
          <tr>\
            <td>' + Drupal.t('Color') + ':</td>\
            <td><table><tr></tr></table></td>\
          </tr>\
        </tbody>\
      </table>';
    wrapper.append(edit_color)
    jQuery.each(colors, function(code, data) {
      wrapper.find("table.color table tr").append(
        '<td><div class="check ' + (color == code ? 'current' : '') + '" onclick="Drupal.yandex_maps.actions.check(this)"><span alt="' + code + '" style="background-color: ' + data.hex + ';" title="' + Drupal.t(data.title) + '"></span></div></td>'
      );
    });

    // Prepare thickness and opacity selectors.
    if (type != 'placemark') {
      var edit_so =
        '<table class="thickness_opacity">\
          <tbody>\
            <tr>\
              <td class="ttl thickness">' + Drupal.t('Thickness') + ':</td>\
              <td><table class="thickness"><tbody><tr></tr></tbody></table></td>\
              <td class="ttl opacity">' + Drupal.t('Transparent') + ':</td>\
              <td><table class="opacity"><tbody><tr></tr></tbody></table></td>\
            </tr>\
          </tbody>\
        </table>';
      wrapper.append(edit_so);

      jQuery.each([1, 5, 10], function(i, px) {
        wrapper.find("table.thickness tr").append(
          '<td class="thick' + px + '">\
            <div onclick="Drupal.yandex_maps.actions.check(this)" class="check'+ (thickness == px ? ' current' : '') + '" alt="' + px + '" title="' + px + 'px">\
              <div class="wrap-thickness"><div></div></div>\
            </div>\
          </td>'
        );
      });

      jQuery.each([10, 25, 50], function(i, percent) {
        wrapper.find("table.opacity tr").append(
          '<td class="opac' + percent + '">\
            <div onclick="Drupal.yandex_maps.actions.check(this)" class="check' + (opacity == percent ? ' current' : '') + '" alt="' + percent + '" title="' + percent + '%">\
              <div class="opacity-wrap"></div>\
            </div>\
          </td>'
        );
      });
    }

    // Prepare save and delete buttons.
    var edit_buttons =
      '<div class="footer">\
        <span onclick="Drupal.yandex_maps.actions.save(this,\'' + map_id + '\',' + object_id + ',\'' + type + '\')" class="ok">' + Drupal.t('OK') + '</span>\
        <span onclick="Drupal.yandex_maps.actions.del(this,\'' + map_id + '\',' + object_id + ',\'' + type + '\')" class="delete">' + Drupal.t('Delete') + '</span>\
      </div>';
    wrapper.append(edit_buttons);

    return jQuery("<div/>").append(wrapper).html();
  }

  // Create base geoobject.
  map.geoobject = function(values) {
    var object = null;
    var o = jQuery.extend({}, map.geoobject_default_options);
    var p = jQuery.extend({}, map.geoobject_default_properties);
    switch (values.type) {
      case 'placemark':
        object = new ymaps.Placemark(values.coordinates, p, o);
        break;

      case 'line':
        object = new ymaps.Polyline(values.coordinates, p, o);
        break;

      case 'polygon':
        object = new ymaps.Polygon(values.coordinates, p, o);
        break;

      case 'circle':
        object = new ymaps.Circle([values.coordinates, parseInt(values.icon)], p, o);
        break;

      case 'rectangle':
        object = new ymaps.Rectangle(values.coordinates, p, o);
        break;
    }

    // Check description for views implementation.
    if (settings.mode == 'views' && values.entity_id) {
      var template = jQuery('.' + map.id + '_' + values.entity_id);
      template.find(".views-field-geoobjects .field-content").html(values.description);
      values.description = template.html() || '';
      template.hide();
    }

    // Set default values.
    object.properties.set('drupal', {
      map_id:      map.id,
      object_id:   map.content.getLength(),
      type:        values.type,
      description: values.description,
      icon:        values.icon,
      color:       values.color,
      thickness:   values.thickness,
      opacity:     values.opacity
    });

    map.geoobject_update(object);
    map.content.add(object);
    map.object.geoObjects.add(map.content);

    if (settings.mode == 'edit') {
      object.properties.set('balloonContent', map.baloon_template(object));

      // Update map on any changes.
      object.events.add(['statechange', 'optionschange', 'dragend'], function(e) {
        map.update();
      });

      if (values.old == false) {
        if (values.type == 'line' || values.type == 'polygon') {
          object.editor.startDrawing();
          object.editor.events.add(['statechange'], function(e) {
            var eobject = e.originalEvent.target;
            if (eobject.state.get('drawing') == false && eobject.state.get('editing') == true) {
               object.balloon.open();
            }
            map.update();
          });
        }
        else {
          object.balloon.open();
        }
      }
    }
    else if (values.description.length > 0) {
      object.properties.set('balloonContent', values.description);
    }
  }

  // Update geoobject on map.
  map.geoobject_update = function(geoobject) {
    var values = geoobject.properties.get('drupal');

    geoobject.options.set('strokeWidth', values.thickness);

    if (values.type == 'placemark') {
      geoobject.options.set('preset', 'twirl#' + values.color + (values.icon.length == 0 ? 'DotIcon' : (values.icon.length < 3 ? 'Icon' : 'StretchyIcon')));
    }
    else if (values.type == 'line') {
      geoobject.options.set('strokeColor', map.colors[values.color].hex);
      geoobject.options.set('strokeOpacity', (values.opacity / 100));
    }
    else {
      geoobject.options.set('fillOpacity', (values.opacity / 100));
      geoobject.options.set('fillColor', map.colors[values.color].hex);
      if (values.type == 'circle') {
        geoobject.geometry.setRadius(parseInt(values.icon));
      }
    }

    geoobject.properties.set('iconContent', values.icon);
  }

  // Save all data to textarea.
  map.update = function() {
    // Add basic map info.
    var all = {
      content: [],
      zoom: map.object.getZoom(),
      center: map.object.getCenter().toString(),
      type: map.object.getType().split('#')[1]
    }

    // Add all geoobjects.
    map.content.each(function(object){
      all.content.push({
        type:        object.properties.get('drupal.type'),
        coordinates: object.geometry.getCoordinates(),
        description: object.properties.get('drupal.description'),
        color:       object.properties.get('drupal.color'),
        icon:        object.properties.get('drupal.icon'),
        opacity:     object.properties.get('drupal.opacity'),
        thickness:   object.properties.get('drupal.thickness')
      });
    });

    var field = jQuery('#' + map.id + '_settings');
    field.html(JSON.stringify(all));
  }

  // Add items to map.
  if (settings.elements) {
    jQuery.each(settings.elements, function(k, element) {
      if (element.coordinates) {
        element.old = true;
        map.geoobject(element);
      }
    });
    map.update();
  }

  map.object.events.add(['boundschange', 'typechange'], function() {
    map.update();
  });

  map.object.events.add('click', function (e) {
    if (settings.mode == 'edit') {
      var type = false, coordinates = e.get('coordPosition');
      var additional = function() {};

      if (map.editTools.tools.placemark && map.editTools.tools.placemark.isSelected()) {
        type = 'placemark';
      }
      else if (map.editTools.tools.line && map.editTools.tools.line.isSelected()) {
        type = 'line';
        coordinates = [coordinates];
      }
      else if (map.editTools.tools.polygon && map.editTools.tools.polygon.isSelected()) {
        type = 'polygon';
        coordinates = [[coordinates]];
      }
      else if (map.editTools.tools.circle && map.editTools.tools.circle.isSelected()) {
        type = 'circle';
      }
      else if (map.editTools.tools.rectangle && map.editTools.tools.rectangle.isSelected()) {
        type = 'rectangle';
        coordinates = map.object.getBounds();
      }
      else {
        return;
      }

      map.geoobject({
        coordinates: coordinates,
        type: type,
        description: '',
        icon: type == 'circle' ? parseInt((100000000 / (Math.pow(map.object.getZoom(), 4)))) : '',
        color: 'red',
        thickness: 5,
        opacity: 50,
        old: false
      });

      if (type == 'rectangle') {
        map.object.setZoom(map.object.getZoom() - 1);
      }

      map.editTools.tools[type].deselect();
      map.update();
    }
  });

  this.object.map = map;
  return this;
}

Drupal.yandex_maps.actions = Drupal.yandex_maps.actions || {};

Drupal.yandex_maps.actions.save = function(e, map_id, object_id, type) {
  var balloon = jQuery(e).parents("div.ymap_balloon");
  var map = Drupal.yandex_maps.maps[map_id];

  // Get geobject.
  var object;
  map.content.each(function(e) {
    if (e.properties.get('drupal.object_id') == object_id) {
      object = e;
    }
  });

  // Parse variables.
  var icon = balloon.find("input").val() || '';
  var color = balloon.find("table.color .current span").attr('alt') || 'red';
  var opacity = parseInt(balloon.find("table.opacity .current").attr('alt')) || 50;
  var thickness = balloon.find("table.thickness .current").attr('alt') || 5;
  var desc = balloon.find("textarea").val() || '';

  // Save properties.
  object.properties.set('drupal.icon', icon);
  object.properties.set('drupal.color', color);
  object.properties.set('drupal.opacity', opacity);
  object.properties.set('drupal.thickness', thickness);
  object.properties.set('drupal.description', desc);

  object.properties.set('balloonContent', map.baloon_template(object));
  object.balloon.close();

  map.geoobject_update(object);
  Drupal.yandex_maps.maps[map_id].update();
}

Drupal.yandex_maps.actions.del = function(e, map_id, object_id, type) {
  var map = Drupal.yandex_maps.maps[map_id];
  map.content.each(function(e) {
    if (e.properties.get('drupal.object_id') == object_id) {
      object = e;
    }
  });
  object.getMap().geoObjects.remove(object)
  map.content.remove(object);
  map.update();
}

Drupal.yandex_maps.actions.check = function(e) {
  jQuery(e).parent().parent().find(".check").removeClass('current');
  jQuery(e).addClass('current');
}

Drupal.yandex_maps.actions.description = function(e, show) {
  var label = jQuery(e).parent().find("label.hint");
  var val = jQuery(e).val().length;
  if (show) {
    label.hide();
  }
  else if (!val) {
    label.show();
  }
}
