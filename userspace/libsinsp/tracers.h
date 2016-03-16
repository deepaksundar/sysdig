/*
Copyright (C) 2013-2014 Draios inc.

This file is part of sysdig.

sysdig is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.

sysdig is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sysdig.  If not, see <http://www.gnu.org/licenses/>.
*/

#define UESTORAGE_INITIAL_BUFSIZE 256

///////////////////////////////////////////////////////////////////////////////
// A partial tracer
///////////////////////////////////////////////////////////////////////////////
class sinsp_partial_tracer
{
public:
	sinsp_partial_tracer()
	{
		m_tags_storage = (char*)malloc(UESTORAGE_INITIAL_BUFSIZE);
		m_argnames_storage = (char*)malloc(UESTORAGE_INITIAL_BUFSIZE);
		m_argvals_storage = (char*)malloc(UESTORAGE_INITIAL_BUFSIZE);
		m_tags_storage_size = UESTORAGE_INITIAL_BUFSIZE;
		m_argnames_storage_size = UESTORAGE_INITIAL_BUFSIZE;
		m_argvals_storage_size = UESTORAGE_INITIAL_BUFSIZE;
	}

	~sinsp_partial_tracer()
	{
		if(m_tags_storage)
		{
			free(m_tags_storage);
		}

		if(m_argnames_storage)
		{
			free(m_argnames_storage); 
		}

		if(m_argvals_storage)
		{
			free(m_argvals_storage); 
		}
	}

	inline bool compare(sinsp_partial_tracer* other)
	{
		if(m_id != other->m_id)
		{
			return false;
		}

		if(memcmp(m_tags_storage, 
			other->m_tags_storage,
			MIN(m_tags_len, other->m_tags_len)) == 0)
		{
			return true;
		}

		return false;
	}

	char* m_tags_storage;
	char* m_argnames_storage;
	char* m_argvals_storage;
	uint32_t m_tags_len;
	uint32_t m_argnames_len;
	uint32_t m_argvals_len;
	uint32_t m_tags_storage_size;
	uint32_t m_argnames_storage_size;
	uint32_t m_argvals_storage_size;
	uint64_t m_id;
	vector<char*> m_tags;
	vector<char*> m_argnames;
	vector<char*> m_argvals;
	vector<uint32_t> m_taglens;
	vector<uint32_t> m_argnamelens;
	vector<uint32_t> m_argvallens;
	uint32_t m_ntags;
	uint32_t m_nargs;

	uint64_t m_time;
};

///////////////////////////////////////////////////////////////////////////////
// tracer parser
///////////////////////////////////////////////////////////////////////////////
class sinsp_tracerparser
{
public:
	enum parse_result
	{
		RES_OK = 0,
		RES_COMMA = 1,
		RES_FAILED = 2,
		RES_TRUNCATED = 3,
	};

	sinsp_tracerparser(sinsp *inspector);
	~sinsp_tracerparser();
	uint32_t get_storage_size()
	{
		return m_storage_size;
	}
	void set_storage_size(uint32_t newsize);
	parse_result process_event_data(char *data, uint32_t datalen, uint64_t ts);
	inline void parse(char* evtstr, uint32_t evtstrlen);
	inline void parse_simple(char* evtstr, uint32_t evtstrlen);
	void test();

	char* m_type_str;
	int64_t m_id;
	vector<char*> m_tags;
	vector<char*> m_argnames;
	vector<char*> m_argvals;
	vector<uint32_t> m_taglens;
	vector<uint32_t> m_argnamelens;
	vector<uint32_t> m_argvallens;
	pair<vector<char*>*, vector<char*>*> m_args;
	uint32_t m_tot_taglens;
	uint32_t m_tot_argnamelens;
	uint32_t m_tot_argvallens;
	sinsp_partial_tracer* m_enter_pae;
	sinsp_partial_tracer m_exit_pae;
	sinsp_threadinfo* m_tinfo;

VISIBILITY_PRIVATE
	inline parse_result skip_spaces(char* p, uint32_t* delta);
	inline parse_result skip_spaces_and_commas(char* p, uint32_t* delta, uint32_t n_expected_commas);
	inline parse_result skip_spaces_and_char(char* p, uint32_t* delta, char char_to_skip);
	inline parse_result skip_spaces_and_commas_and_sq_brakets(char* p, uint32_t* delta);
	inline parse_result skip_spaces_and_commas_and_cr_brakets(char* p, uint32_t* delta);
	inline parse_result skip_spaces_and_commas_and_all_brakets(char* p, uint32_t* delta);
	inline parse_result parsestr(char* p, char** res, uint32_t* delta);
	inline parse_result parsestr_not_enforce(char* p, char** res, uint32_t* delta);
	inline parse_result parsenumber(char* p, int64_t* res, uint32_t* delta);
	inline parse_result parsenumber_colend(char* p, int64_t* res, uint32_t* delta);
	inline void init_partial_tracer(sinsp_partial_tracer* pae);

	sinsp *m_inspector;
	char* m_storage;
	uint32_t m_storage_size;
	uint32_t m_fragment_size;
	sinsp_tracerparser::parse_result m_res;
	string m_fullfragment_storage_str;

	friend class sinsp_parser;
};