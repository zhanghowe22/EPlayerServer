#pragma once
#include "Public.h"
#include <map>
#include <list>
#include <vector>

class _Table_;
using KeyValue = std::map<Buffer, Buffer>;
using Result = std::list<_Table_>;

// ���ݿ�ͻ��˵�ͳһ�����淶�ӿ���
class CDatabaseClient
{
public:
	CDatabaseClient() {}
	virtual ~CDatabaseClient() {}

public:
	CDatabaseClient(const CDatabaseClient&) = delete;
	CDatabaseClient& operator=(const CDatabaseClient&) = delete;

public:
	// ����
	virtual int Connect(const KeyValue& args) = 0;
	// �޽����ִ��
	virtual int Exec(const Buffer& sql) = 0;
	// �н����ִ��
	virtual int Exec(const Buffer& sql, Result& result, const _Table_& table) = 0;
	// ��������
	virtual int StartTransaction() = 0;
	// �ύ����
	virtual int CommitTransaction() = 0;
	// �ع�����
	virtual int RollbackTransaction() = 0;
	// �ر�����
	virtual int Close() = 0;
	// �Ƿ�����
	virtual bool IsConnected() = 0;
};

class _Field_;
using PField = std::shared_ptr<_Field_>;
using FieldArray = std::vector<PField>;
using FieldMap = std::map<Buffer, PField>;
class _Table_;
using PTable = std::shared_ptr<_Table_>;

// ��Ļ���
class _Table_ {
public:
	_Table_() {}
	virtual ~_Table_() {}

	// ����Create�������SQL���,����ִ�л��������ݿ��У�����ֻ������һ�����
	virtual Buffer Create() = 0;
	// ɾ����
	virtual Buffer Drop() = 0;
	// ��ɾ�Ĳ�������
	virtual Buffer Insert(const _Table_& values) = 0;
	virtual Buffer Delete() = 0;
	virtual Buffer Modify() = 0; // TODO: ���������Ż�
	virtual Buffer Query() = 0;
	// ����һ�����ڱ�Ķ���
	virtual PTable Copy() = 0;
public:
	// ��ȡ���ȫ��
	virtual operator const Buffer() const = 0;

public:
	// ��������DB������
	Buffer Database;
	Buffer Name;
	FieldArray FieldDefine; // �еĶ��壨�洢��ѯ�����
	FieldMap Fields; // �еĶ���ӳ���
};

// �еĻ���
class _Field_ {
public:
	_Field_() {}
	_Field_(const _Field_& field) {
		Name = field.Name;
		Type = field.Type;
		Attr = field.Attr;
		Default = field.Default;
		Check = field.Check;
	}
	virtual _Field_& operator=(const _Field_& field) {
		if (this != &field) {
			Name = field.Name;
			Type = field.Type;
			Attr = field.Attr;
			Default = field.Default;
			Check = field.Check;
		}
		return *this;
	}
	virtual ~_Field_() {}

public:
	virtual int Create() = 0;
	virtual void LoadFromStr(const Buffer& str) = 0;
	// where���ʹ�õ�
	virtual Buffer toEqualExp() const = 0;
	virtual Buffer toSqlStr() const = 0;
	// �е�ȫ��
	virtual operator const Buffer() const = 0;

public:
	Buffer Name;
	Buffer Type;
	Buffer Size;
	unsigned Attr;
	Buffer Default;
	Buffer Check;
};