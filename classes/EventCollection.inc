<?php

/**
 * @file defines class for event collection
 */

class EventCollection {

  public $events;

  function __construct($events) {
    $this->events = $events;
  }

  public static function date_period($first_date, $last_date) {

    $begin = $first_date;
    $end = $last_date;
    $end->modify( '+1 day' );
    $interval = new DateInterval('P1D');
    $calendar_period = new DatePeriod($begin, $interval, $end);

    return $calendar_period;
  }

  public function chunked_events($date_table, $interval) {

    $func = 'comparison_' . $interval . '_string';

    $chunked_events = Array();
    foreach ($date_table as $timestamp => $event_array) {
      $chunked_index = CalDate::$func($event_array[0]['date_object']);
      $chunked_events[$chunked_index][$timestamp] = $event_array;
    }

    return $chunked_events;
  }

  public function date_table() {

    $date_table = NULL;

    foreach ($this->events as $key => $event) {

      $event_date_table = $event->date_table;

      if ($date_table === NULL) {
        $date_table = $event_date_table;
      }
      else {
        $date_table = array_merge_recursive($date_table, $event_date_table);
      }

    }

    // order events by date
    ksort($date_table);

    // order events by start time
    foreach ($date_table as $key => $date) {
      usort($date, function($a, $b) {
        if ($a['time_string'] == $b['time_string']) {
          return 0;
        }
        return ($a['time_string'] < $b['time_string']) ? -1 : 1;
      });
      $date_table[$key] = $date;
    }

    return $date_table;

  }
}
