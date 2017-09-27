<?php

/**
 * @file contains theme implementation for the RBKC Calendar
 */

/**
 * Implements hook_theme();
 */
function rbkc_festival_calendars_theme() {
  return array(
    'calendarbutton' => array(
      'variables' => array(
        'date' => NULL,
        'interval' => 'day',
      ),
    ),
    'calendartablehead' => array(
      'variables' => array(
        'interval' => NULL,
        'headers' => NULL,
      ),
    ),
    'calendartablerow' => array(
      'variables' => array(
        'interval' => NULL,
        'date' => NULL,
        'url' => NULL,
        'cols' => [],
        'first_on_date' => FALSE,
      ),
    ),
    'calendarentry' => array(
      'variables' => array(
        'date' => NULL,
        'interval' => NULL,
        'is_active' => FALSE,
        'button' => NULL,
        'head' => NULL,
        'rows' => NULL,
      ),
    ),
  );
}

/**
 * Implements theme()
 */
function theme_calendarbutton($variables) {
  $button ='<button class="event-calendar_button" type="button"><span>' . $variables['buttondate'] . '</span>' . $variables['toggle-icon'] . '</button>';
  return $button;
}

/**
 * Implements theme()
 */
function theme_calendartablerow($variables) {
  $cols = $variables['cols'];
  $row_class = $variables['row_class'];

  $row = "<tr class='" . $row_class . "'>";
    foreach ($cols as $key => $col) {
      $row .= '<td><span>' . $col . '</span></td>';
    }
  $row .= "</tr>";

  return $row;
}

/**
 * Implements theme()
 */
function theme_calendartablehead($variables) {
  $cols = $variables['headers'];

  $row = "<tr class='element-invisible'>";
    foreach ($cols as $key => $col) {
      $row .= '<th>' . $col . '</th>';
    }
  $row .= "</tr>";

  return $row;
}

/**
 * Implements theme()
 */
function theme_calendarentry($variables) {

  $button = $variables['button'];
  $head = $variables['head'];
  $rows = $variables['rows'];
  $calendar_entry_classes = $variables['calendar_entry_classes'];
  $calendar_table_classes = $variables['calendar_table_classes'];

  $table_head = '<thead>';
  $table_head .= $head;
  $table_head .= '</thead>';

  $table_body = '<tbody>';
  $table_body .= $rows;
  $table_body .= '</tbody>';

  $table = '<table>';
  $table .= $table_head;
  $table .= $table_body;
  $table .= '</table>';

  $calendar_entry = '<div class="' . $calendar_entry_classes . '">';
  $calendar_entry .= $button;
  $calendar_entry .= '<div class="' . $calendar_table_classes . '">';
  $calendar_entry .= $table;
  $calendar_entry .= '</div>';
  $calendar_entry .= '</div>';

  return $calendar_entry;

}

/**
 * Implements hook_preprocess();
 */
function rbkc_festival_calendars_preprocess_calendarbutton(&$variables) {

  $calendar_date = clone $variables['date'];
  // create classes variable
  $today = new DateTime();
  $buttondate = NULL;

  global $theme;
  $path_to_theme = drupal_get_path('theme', $theme);
  $variables['toggle-icon']  = '<svg><use xlink:href="' . base_path() . $path_to_theme . '/dist/images/sprites.svg' . '#chevron-down"></use></svg></a>';


  if ($variables['interval'] == 'day') {
    $buttondate = $calendar_date->format('l jS');
  }

  if ($variables['interval'] == 'week') {

    $week_end = NULL;
    $date_iterator = clone $calendar_date;

    for ($i = 0; $i < 7; $i++) {
      if ($date_iterator->format('D') == "Sun") {
        $week_end = $date_iterator;
        break;
      }
      $date_iterator->modify( '+1 day' );
    }

    $calendar_begin_month = $calendar_date->format('m');
    $calendar_end_month = $week_end->format('m');
     // we want to show the date at the start of the calendar range if it's different to the date at the end
    if ($calendar_begin_month == $calendar_end_month) {
      $button_begin =  $calendar_date->format('j');
    }
    else {
      $button_begin =  $calendar_date->format('j F');
    }

    $button_end = $week_end->format('j F');
    $buttondate = $button_begin . ' - ' . $button_end;
  }

  if ($variables['interval'] == 'month') {

    $calendar_year = $calendar_date->format('Y');
    $include_year = $today->format('Y') == $calendar_year ? NULL : $calendar_year;
    $buttondate = $include_year ? $calendar_date->format('F Y') : $calendar_date->format('F');
  }

  $variables['buttondate'] = $buttondate;

}

/**
 * Implements hook_preprocess();
 */
function rbkc_festival_calendars_preprocess_calendartablerow(&$variables) {

  $celldata = $variables['cols'];

  $celldata['name'] = l($celldata['name'], $variables['url'], $options = Array('html' => TRUE));

  // create special class for rows that show first events on a date (not relevant for daily calendar)
  if ($variables['interval'] == 'week' || $variables['interval'] == 'month') {
    $formatted_date = $variables['date']->format('D j');

    $date = Array('date' => $formatted_date);

    $celldata = $date + $celldata;

    if ($variables['first_on_date']) {
      $variables['row_class'] = 'first';
    }
    else {
      $variables['row_class'] = 'hide-dates';
    }
  }

  $variables['cols'] = $celldata;

}

/**
 * Implements hook_preprocess();
 */
function rbkc_festival_calendars_preprocess_calendartablehead(&$variables) {

  if ($variables['interval'] == 'week' || $variables['interval'] == 'month') {
     array_unshift($variables['headers'], 'date');
  }
}

/**
 * Implements hook_preprocess();
 */
function rbkc_festival_calendars_preprocess_calendarentry(&$variables) {

  $variables['calendar_table_classes'][] = "event-calendar_table";
  $variables['calendar_entry_classes'][] = 'event-calendar_entry';

  $interval_class = "event-calendar_by-" . $variables['interval'];
  $variables['calendar_entry_classes'][] = $interval_class;

  $is_active = $variables['is_active'];
  $active_class = $is_active ? "active" : "inactive";
  $active_transition_class = $is_active ? "active-transition" : "inactive-transition";
  $variables['calendar_entry_classes'][] = $active_class;
  $variables['calendar_entry_classes'][] = $active_transition_class;

}

/**
 * Implements hook_process();
 */
function rbkc_festival_calendars_process_calendarentry(&$variables) {

  $table_classes_string = "";
  foreach ($variables['calendar_table_classes'] as $class) {
    $table_classes_string .= " " . $class;
  }
  $variables['calendar_table_classes'] = $table_classes_string;

  $entry_classes_string = "";
  foreach ($variables['calendar_entry_classes'] as $class) {
    $entry_classes_string .= " " . $class;
  }
  $variables['calendar_entry_classes'] = $entry_classes_string;

}
