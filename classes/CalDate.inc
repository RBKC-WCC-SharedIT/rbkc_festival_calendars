<?php

/**
 * @file defines class for calendar date
 */

class CalDate {

  public $date;

  function __construct($date) {
    $this->date = new DateTime($date);
  }

  public static function comparison_string($date) {
    return $date->format('Y-m-d');
  }

  public static function comparison_month_string($date) {
    return $date->format('Y-m');
  }

  public static function comparison_week_string($date) {
    return $date->format('Y-W');
  }

}

class CalTime extends CalDate {

  public function __toString() {
    return $this->date->format('G:i');
  }

  public static function comparison_string($date) {
    return $date->format('G-i');
  }

}
