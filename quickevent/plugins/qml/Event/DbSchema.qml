import QtQml 2.0
import qf.core 1.0
import qf.core.sql.def 1.0

Schema {
	name: 'untitled'
	tables: [
		Table { name: 'config'
			fields: [
				Field { name: 'ckey'; type: String {} },
				Field { name: 'cvalue'; type: String {} }
			]
			indexes: [
				Index {fields: ['ckey']; primary: true }
			]
		},
		Table { name: 'event'
			fields: [
				Field { name: 'id'; type: Serial {} },
				Field { name: 'stageCount'; type: Int { } },
				Field { name: 'name'; type: String {} },
				Field { name: 'description'; type: String {} },
				Field { name: 'date'; type: Date {} },
				Field { name: 'place'; type: String {} },
				Field { name: 'mainReferee'; type: String {} },
				Field { name: 'director'; type: String {} },
				Field { name: 'importId'; type: String {} }
			]
			indexes: [
				Index {fields: ['id']; primary: true }
			]
		},
		Table { name: 'competitors'
			fields: [
				Field { name: 'id'; type: Serial {} },
				Field { name: 'startNumber'; type: Int {} },
				Field { name: 'classId'; type: String { length: 10 } },
				Field { name: 'courseId'; type: String { length: 10 } },
				Field { name: 'firstName'; type: String {} },
				Field { name: 'lastName'; type: String {} },
				Field { name: 'registration'; type: String { length: 10 } },
				Field { name: 'licence'; type: String { length: 1 } },
				Field { name: 'club'; type: String { } },
				Field { name: 'country'; type: String { } },
				Field { name: 'siId'; type: Int { } },
				Field { name: 'relayId'; type: Int { } },
				Field { name: 'leg'; type: Int { } },
				Field { name: 'note'; type: String { } },
				Field { name: 'ranking'; type: Int { } },
				Field { name: 'importId'; type: String {} }
			]
			indexes: [
				Index {fields: ['id']; primary: true },
				Index {fields: ['classId'] },
				Index {fields: ['importId'] }
			]
		},
		Table { name: 'laps'
			fields: [
				Field { name: 'id'; type: Serial {} },
				Field { name: 'competitorId'; type: Int {} },
				Field { name: 'siId'; type: Int {} },
				Field { name: 'stageId'; type: Int {} },
				Field { name: 'startTime'; type: Int {}
					comment: 'in seconds'
				},
				Field { name: 'lapTime'; type: Int {}
					comment: 'in seconds'
				},
				Field { name: 'lapTimeMS'; type: Int {}
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
				Field { name: 'status';
					type: Enum { name: 'enum_lap_status';
						keys: ['OFF', 'START', 'FINISH']
					}
					defaultValue: 'OFF';
					notNull: true
				}
			]
			indexes: [
				Index { fields: ['id']; primary: true },
				Index { fields: ['competitorId'] },
                Index { fields: ['stageId, competitorId']; unique: true },
				Index { fields: ['status', 'lapTime', 'lapTimeMS'] }
			]
		},
		Table { name: 'registrations'
			fields: [
				Field { name: 'id'; type: Serial {} },
				Field { name: 'firstName'; type: String {} },
				Field { name: 'lastName'; type: String {} },
				Field { name: 'registration'; type: String { length: 10 } },
				Field { name: 'clubId'; type: String { } },
				Field { name: 'country'; type: String { } },
				Field { name: 'siId'; type: Int { } }
			]
			indexes: [
				Index {fields: ['id']; primary: true },
				Index {fields: ['registration'] }
			]
		},
		Table { name: 'stages'
			fields: [
				Field { name: 'id'; type: Int {} },
				Field { name: 'startTime'; type: Time {} }
			]
			indexes: [
				Index {fields: ['id']; primary: true }
			]
		},
		Table { name: 'classes'
			fields: [
				Field { name: 'id'; type: String { length: 10 } },
				Field { name: 'courseId'; type: String { length: 10 } },
				Field { name: 'name'; type: String { length: 10 } }
			]
			indexes: [
				Index {fields: ['id']; primary: true }
			]
		},
		Table { name: 'cards'
			fields: [
				Field {
					name: 'id'
					type: Serial {}
				},
				Field { name: 'stationNumber'
					type: Int {
						length: 3
						unsigned: true
					}
					//notNull: true
					defaultValue: 0
				},
				Field { name: 'idsi'
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
					comment: 'JSON of format [[code, time], ...]}'
				}
  			]
			indexes: [
				Index {
					fields: ['id']
					primary: true
				},
				Index {
					fields: ['idsi']
					unique: false
				}
			]
		}
	]
	Component.onCompleted: {
		Log.info("DbSchema created");
	}

}
