<?php
/*
 * Migration class to import profiles to profile2.
 */
class JieyyalMigration extends Migration {
  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = t('Migrate Articles for listerine.');
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'nid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        )
      ),
      MigrateDestinationNode::getKeySchema()
    );
    $this->path = "C:\Users\jieyyal\Sites\listerine_node.csv";
    $this->csvcolumns = array();
    $this->source = new MigrateSourceCSV($this->path, $this->csvcolumns, array('header_rows' => 1));
    $this->destination = new MigrateDestinationNode('article');
    // Assign mappings TO destination fields FROM source fields. To discover
    $this->addFieldMapping('nid', 'nid');
    $this->addFieldMapping('vid', 'vid');
    $this->addFieldMapping('uid', 'uid');
    $this->addFieldMapping('title_field', 'title');
    $this->addFieldMapping('title_field:language', 'title_language');
    $this->addFieldMapping('body', 'body');
    $this->addFieldMapping('body:format', 'body_format');
    $this->addFieldMapping('body:language', 'body_language');
    $this->addFieldMapping('metatag_title', 'meta_title');
    $this->addFieldMapping('metatag_description', 'meta_description');
    $this->addFieldMapping('metatag_keywords', 'meta_keywords');
    $this->addFieldMapping('language', 'language');
//    $this->addFieldMapping('path', 'url');
//    $this->addFieldMapping('pathauto', 'pathauto');
  }
  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
    // Assumes the source columns are english_name and french_name.
    $row->title = array($row->title_en, $row->title_zh);
    $row->title_language = array('en', 'zh-hans');
    
    $row->body = array($row->body_en, $row->body_zh);
    $row->body_language = array('en', 'zh-hans');
    $row->body_format = 'full_html';

    return TRUE;
  }
  public function complete($entity, stdClass $row) {

    $url_alias_results_en = db_select('url_alias', 'ua')
        ->condition('source', 'node/' . $entity->nid)
        ->condition('language', 'en')
        ->fields('ua')
        ->execute();
    $url_alias_results_zh = db_select('url_alias', 'ua')
        ->condition('source', 'node/' . $entity->nid)
        ->condition('language', 'zh-hans')
        ->fields('ua')
        ->execute();
    if ($url_alias_results_en->rowCount() == 1 && $url_alias_results_zh->rowCount() == 0) {
      //if path is the same for both languages then adding back the english path
      db_insert('url_alias')
        ->fields(array(
          'source' =>'node/' . $entity->nid,
          'alias' => $row->url,
          'language' => 'zh-hans',
        ))
        ->execute();
    }
    elseif ($url_alias_results_en->rowCount() == 1 && $url_alias_results_zh->rowCount() == 1) {
      db_update('url_alias')
        ->fields(array(
          'alias' => $row->url,
        ))
        ->condition('source', 'node/' . $entity->nid)
        ->condition('language', 'zh-hans')
        ->execute();
    }
    else {
      // Something must be wrong.
      return FALSE;
    }

    // Add a translation into the system only if it exists.
    if (!is_null($entity->title_field['zh-hans'][0]['value'])) {

      // Create a record for the french translation in the entity translation table.
      $nid = db_insert('entity_translation')
             ->fields(array(
                      'entity_type' => 'node',
                      'entity_id' => $entity->nid,
                      'language' => 'zh-hans',
                      'source' => 'en',
                      'uid' => $entity->uid,
                      'status' => $entity->status,
                      'translate' => $entity->translate,
                      'created' => $entity->created,
                      'changed' => $entity->changed,
                     ))
             ->execute();
    }
  }
}