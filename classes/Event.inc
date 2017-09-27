<?php

/**
 * @file defines class for event
 */

class Event {

  public $url;
  public $date_ranges;
  public $view_fields;

  function __construct($date_ranges, $name = NULL, $url = NULL, $view_fields = NULL) {
    $this->date_ranges = $date_ranges;
    $this->name = $name;
    $this->url = $url;
    $this->view_fields = $view_fields;
    $this->date_table = $this->create_date_table($this->date_ranges);
  }

  private function create_date_table($date_ranges) {
    $date_table = Array();
    foreach ($date_ranges as $range) {

      $range_period =  EventCollection::date_period($range['date_from']->date, $range['date_to']->date);
      $start_time = (string) $range['time_from'];

      foreach ($range_period as $date) {
        $datestring = CalDate::comparison_string($date);
        $event_instance = Array();
        $event_instance['date_object'] = $date;
        $event_instance['calendar_data'] = $this->event_details($range);
        $event_instance['time_string'] = $this->times_string($range);
        $event_instance['display_string'] = $this->long_date_string($date) . ', ' . $this->times_string($range);
        $date_table[$datestring][] = $event_instance;
      }
    }

    return $date_table;
  }

  public function get_sorted_dates() {
    $date_table = $this->date_table;
    ksort($date_table);
    return $date_table;
  }

  public function long_date_string($date) {
    return $date->format('l j F Y');
  }

  public function short_date_string($date) {
    return $date->format('D j');
  }

  public function times_string($date_range) {
    $time_from = (string) $date_range['time_from'];
    $time_to = (string) $date_range['time_to'];
    return $time_from . " - " . $time_to;
  }

  /** Returns event details for calendar display
    *
    * The keys defined in the returned array will be column
    * headers in the rendered data table
    */
  private function event_details($date_range)  {
    $details =  Array(
      'times' => Array(
        'time_from' => $date_range['time_from'],
        'time_to' => $date_range['time_to'],
       ),
      'headers' => Array(),
      'fields' => Array(),
    );

    $details['headers'][] = 'name';
    $details['url'] = $this->url;

    $details['fields']['name'] = $this->name;

    foreach ($this->view_fields as $key => $field) {
      $details['headers'][] = $field['header'];

      if ($key === 'event_times') {
        $details['fields'][$key] = $this->times_string($date_range);
      }
      else {
        $details['fields'][$key] = $field['markup'];
      }
    }

    return $details;

  }

}
