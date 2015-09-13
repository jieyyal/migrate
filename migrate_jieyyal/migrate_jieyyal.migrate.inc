<?php
/**
 * @file
 * Declares our migrations.
 */
/**
 * Implements hook_migrate_api().
 */
function migrate_jieyyal_migrate_api() {
  $api = array(
    'api' => 2,
    'groups' => array(
      'jieyyal' => array(
        'title' => t('Jieyyal Migrations'),
      ),
    ),
    'migrations' => array(
      'Jieyyal' => array(
        'class_name' => 'JieyyalMigration',
        'group_name' => 'jieyyal',
      ),
    ),
    'destination handlers' => array('MigrateTranslationEntityHandler'),
  );
  return $api;
}
