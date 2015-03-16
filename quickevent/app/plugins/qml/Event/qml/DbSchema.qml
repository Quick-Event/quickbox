import QtQml 2.0
import qf.core 1.0
import qf.core.sql.def 1.0

Schema {
	name: 'untitled'
	tables: [
		Table { id: enumz; name: 'enumz'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'groupName'; type: String {} },
				Field { name: 'groupId'; type: String {} },
				Field { name: 'pos'; type: Int {} },
				Field { name: 'caption'; type: String {} },
				Field { name: 'value'; type: String {} }
			]
			indexes: [
				Index {fields: ['groupName' ,'groupId']; unique: true }
			]
		},
		Table { id: config; name: 'config'
			fields: [
				Field { name: 'ckey'; type: String {} },
				Field { name: 'cname'; type: String {} },
				Field { name: 'cvalue'; type: String {} },
				Field { name: 'ctype'; type: String {} }
			]
			indexes: [
				Index { fields: ['ckey']; primary: true }
			]
		},
		Table { name: 'stages'
			fields: [
				Field { name: 'id'; type: Int {} },
				Field { name: 'startTime'; type: Time {} },
				Field { name: 'date'; type: Date {} }
			]
			indexes: [
				Index {fields: ['id']; primary: true }
			]
		},
		Table { name: 'courses'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				//Field { name: 'stageId'; type: Int { } },
				Field { name: 'name'; type: String { length: 10 } },
				Field { name: 'length'; type: Int { } },
				Field { name: 'climb'; type: Int { } }
			]
		},
		Table { name: 'codes'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				//Field { name: 'stageId'; type: Int { } },
				Field { name: 'code'; type: Int { } },
				Field { name: 'broken'; type: Boolean { } }
			]
			indexes: [
				//Index {fields: ['stageId', 'code']; unique: true }
			]
		},
		Table { name: 'coursecodes'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'courseId'; type: Int { } },
				Field { name: 'codeId'; type: Int { } }
			]
			indexes: [
				Index {fields: ['courseId']; references: ForeignKeyReference {table: 'courses'; fields: ['id']; } },
				Index {fields: ['codeId']; references: ForeignKeyReference {table: 'codes'; fields: ['id']; } },
				Index {
					fields: ['courseId', 'codeId']
					unique: true
				}
			]
		},
		Table { name: 'classes'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'name'; type: String { length: 10 } }
			]
		},
		Table { name: 'classdefs'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'classId'; type: Int { } },
				Field { name: 'stageId'; type: Int { } },
				Field { name: 'courseId'; type: Int { } },
				Field { name: 'startTimeMin'; type: Int { } },
				Field { name: 'startIntervalMin'; type: Int { } },
				Field { name: 'vacantEvery'; type: Int { } 
						comment: 'place vacant every n-th competitor in class drawing'
				},
				Field { name: 'vacantsAfter'; type: Int { } 
						comment: 'place n vacants gap before next class in starting list'
				},
				Field { name: 'lastTimeMin'; type: Int { } },
				Field { name: 'mapCount'; type: Int { } }
			]
			indexes: [
				Index {fields: ['stageId']; references: ForeignKeyReference {table: 'stages'; fields: ['id']; } },
				Index {fields: ['classId']; references: ForeignKeyReference {table: 'classes'; fields: ['id']; } },
				Index {fields: ['courseId']; references: ForeignKeyReference {table: 'courses'; fields: ['id']; } }
			]
		},
		Table { name: 'competitors'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'startNumber'; type: Int {} },
				Field { name: 'classId'; type: Int {} },
				Field { name: 'courseId'; type: String { length: 10 } },
				Field { name: 'firstName'; type: String {} },
				Field { name: 'lastName'; type: String {} },
				Field { name: 'registration'; type: String { length: 10 } },
				Field { name: 'licence'; type: String { length: 1 } },
				Field { name: 'club'; type: String { } },
				Field { name: 'country'; type: String { } },
				Field { name: 'siId'; type: Int { } },
				Field { name: 'note'; type: String { } },
				Field { name: 'ranking'; type: Int { } },
				Field { name: 'importId'; type: String {} }
			]
			indexes: [
				Index {fields: ['classId']; references: ForeignKeyReference {table: 'classes'; fields: ['id']; } },
				Index {fields: ['importId'] }
			]
		},
		Table { name: 'laps'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'competitorId'; type: Int {} },
				Field { name: 'siId'; type: Int {} },
				Field { name: 'stageId'; type: Int {} },
				Field { name: 'cardId'; type: Int {} },
				Field { name: 'startTimeMs'; type: Int {}
					comment: 'in miliseconds'
				},
				Field { name: 'lapTimeMs'; type: Int {}
					comment: 'in miliseconds'
				},
				Field { name: 'offRace'; type: Boolean { }
					defaultValue: false;
					notNull: true
				},
				Field { name: 'disqualified'; type: Boolean { }
					defaultValue: false;
					notNull: true
				},
				Field { name: 'cardError'; type: Boolean { }
					defaultValue: false;
					notNull: true
				},
				Field { name: 'cardLent'; type: Boolean { }
					defaultValue: false;
					notNull: true
				},
				Field { name: 'status';
					type: String {}
					defaultValue: 'OFF';
					notNull: true
					comment: "referencing enumz.laps.status"
				}
			]
			indexes: [
				Index {
					fields: ['competitorId'];
					references: ForeignKeyReference {
						table: 'competitors';
						fields: ['id'];
						onUpdate: 'RESTRICT';
						onDelete: 'RESTRICT';
					}
				},
				Index {fields: ['stageId']; references: ForeignKeyReference {table: 'stages'; fields: ['id']; } },
				Index {fields: ['stageId, competitorId']; unique: true },
				Index {fields: ['status', 'lapTimeMs'] }
			]
		},
		Table { name: 'registrations'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'firstName'; type: String {} },
				Field { name: 'lastName'; type: String {} },
				Field { name: 'registration'; type: String { length: 10 } },
				//Field { name: 'clubId'; type: String { } },
				Field { name: 'country'; type: String { } },
				Field { name: 'siId'; type: Int { } }
			]
			indexes: [
				Index {fields: ['registration'] }
			]
		},
		Table { name: 'cards'
			fields: [
				Field {
					name: 'id'
					type: Serial { primaryKey: true }
				},
				Field { name: 'stationNumber'
					type: Int {
						length: 3
						unsigned: true
					}
					//notNull: true
					defaultValue: 0
				},
				Field { name: 'siId'
					type: Int {
						unsigned: true
					}
				},
				Field { name: 'checkTime'
					type: Int {
						unsigned: true
					}
					comment: 'seconds in range 0 - 12 hours'
				},
				Field { name: 'startTime'
					type: Int {
						unsigned: true
					}
					comment: 'seconds in range 0 - 12 hours'
				},
				Field { name: 'finishTime'
					type: Int {
						unsigned: true
					}
					comment: 'seconds in range 0 - 12 hours'
				},
				Field { name: 'punches'
					type: String {
						length: 65536
						charset: 'latin1'
					}
					comment: 'JSON of format [[code, time_sec], ...]}'
				}
  			]
			indexes: [
				Index {
					fields: ['siId']
					unique: false
				}
			]
		}
	]
	inserts: [
		Insert {
			table: enumz
			fields: ['groupName', 'groupId', 'pos']
			rows: [
				['laps.status', 'OFF', 1],
				['laps.status', 'START', 2],
				['laps.status', 'FINISH', 3]
			]
		},
		Insert {
			table: config
			rows: [
				['event.stageCount', qsTr('Stage count'), '0', 'int'],
				['event.name', qsTr('Event name'), '', 'QString'],
				['event.date', qsTr('Event date'), '', 'QDate'],
				['event.description', qsTr('Event description'), '', 'QString'],
				['event.place', qsTr('Event place'), '', 'QString'],
				['event.mainReferee', qsTr('Main referee'), '', 'QString'],
				['event.director', qsTr('Director'), '', 'QString'],
				['event.importId', qsTr('Import ID'), '', 'int']
			]
		}
	]
	Component.onCompleted: {
		Log.info("DbSchema created");
	}

}
