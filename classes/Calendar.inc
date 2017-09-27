<?php

/**
 * @file defines class for calendar
 */

class Calendar {

  private $event_collection;

  function __construct($event_collection) {
    $this->event_collection = $event_collection;
  }

  private function create_themed_calendar_entry($calendar_date, $headers, $rows, $is_active, $interval) {

    $thead_args = Array(
      'interval' => $interval,
      'headers' => $headers,
    );
    $calendar_head = theme('calendartablehead', $thead_args);

    // create button
    $button_args = array(
      'date' => $calendar_date,
      'interval' => $interval
    );
    $button = theme('calendarbutton', $button_args);

    // create calendar entry for this chunk
    $calendarargs = Array(
      'date' => $calendar_date,
      'interval' => $interval,
      'is_active' => $is_active,
      'button' => $button,
      'head' => $calendar_head,
      'rows' => $rows,
    );
    return theme('calendarentry', $calendarargs);
  }

  private function create_day_rows($day, $interval) {

    $rows = "";
    foreach ($day as $index => $event) {

      $first_on_date = $index == 0 ? TRUE : FALSE;
      $trargs = Array('interval' => $interval,
                      'date' => $event['date_object'],
                      'url' => $event['calendar_data']['url'],
                      'first_on_date' => $first_on_date,
                      'cols' => $event['calendar_data']['fields'],
                      );
      $rows .= theme('calendartablerow', $trargs);

    }

    return $rows;
  }

  private function calendar_days($date_table, $interval) {

    $calendar = "";
    $calendar_is_active = FALSE;
    $today = new DateTime();
    $headers = reset($date_table);
    $headers = $headers[0]['calendar_data']['headers'];

    foreach ($date_table as $index => $day) {
      if ($day[0]['date_object'] == $today) {
        $calendar_is_active = TRUE;
      }
      $day_rows = $this->create_day_rows($day, $interval);
      $calendar_date = $day[0]['date_object'];
      $calendar .= $this->create_themed_calendar_entry($calendar_date, $headers, $day_rows, $calendar_is_active, $interval);
    }

    return $calendar;

  }

  private function calendar_chunked($chunks, $interval) {

    $calendar = "";
    $calendar_is_active = FALSE;
    $today = new DateTime();
    $headers = reset(reset($chunks));
    $headers = $headers[0]['calendar_data']['headers'];

    foreach ($chunks as $index => $chunk) {
      $chunk_rows = '';

      foreach ($chunk as $index => $day) {
        $calendar_is_active = $this->is_active($day[0]['date_object'], $interval);
        $dayrows = $this->create_day_rows($day, $interval);
        $chunk_rows .= $dayrows;
      }

      $calendar_date = reset($chunk);
      $calendar_date = $calendar_date[0]['date_object'];
      $calendar .= $this->create_themed_calendar_entry($calendar_date, $headers, $chunk_rows, $calendar_is_active, $interval);
    }

    return $calendar;
  }

  private function is_active(DateTime $calendar_date, $interval) {
    $today = new DateTime();
    $is_active = FALSE;

    switch ($interval) {
      case 'day':
        $is_active = $today == $calendar_date ? TRUE : FALSE;
        break;
      case 'week':
        $is_active = CalDate::comparison_week_string($today) == CalDate::comparison_week_string($calendar_date) ? TRUE : FALSE;
        break;
      case 'month':
        $is_active = CalDate::comparison_month_string($today) == CalDate::comparison_month_string($calendar_date) ? TRUE : FALSE;
        break;
    }

    return $is_active;

  }


  public function print_calendar_by($interval) {

    $date_table = $this->event_collection->date_table();

    if ($interval !== 'day') {
       $dates_by_chunk = $this->event_collection->chunked_events($date_table, $interval);
       print $this->calendar_chunked($dates_by_chunk, $interval);
    }
    else {
      print $this->calendar_days($date_table, $interval);
    }
  }

}
