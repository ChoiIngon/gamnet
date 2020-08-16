#include "Mail.h"
#include "../UserSession.h"
#include "../../idl/MessageCommon.h"
#include "../../idl/MessageLobby.h"
#include "Bag.h"
#include "Item.h"

namespace Component {
	Mail::Mail(const std::shared_ptr<UserSession>& session)
		: last_mail_seq(0)
		, last_update_time(0)
		, session(session)
	{
	}

	void Mail::Load()
	{
		time_t now = time(nullptr);
		if(time(nullptr) < last_update_time + 60)
		{
			return;
		}

		auto rows = Gamnet::Database::MySQL::Execute(session->shard_index,
			"SELECT mail_seq, expire_date, mail_message, item_id, item_count FROM user_mail WHERE user_seq=", session->user_seq, " AND expire_date > now() AND delete_yn='N' and mail_seq>", last_mail_seq
		);

		Message::Lobby::MsgSvrCli_Mail_Ntf ntf;
		for (auto& row : rows)
		{
			std::shared_ptr<MailData> mailData = std::make_shared<MailData>();
			mailData->mail_seq = row->getUInt64("mail_seq");
			mailData->expire_date = row->getString("expire_date");
			mailData->mail_message = row->getString("mail_message");
			mailData->item_id = row->getUInt32("item_id");
			mailData->item_count = row->getInt32("item_count");
			last_mail_seq = std::max(last_mail_seq, mailData->mail_seq);
			mail_datas.insert(std::make_pair(mailData->mail_seq, mailData));

			Message::MailData mail;
			mail.expire_date = Gamnet::Time::UnixTimestamp(mailData->expire_date);
			mail.item_count = mailData->item_count;
			mail.item_id = mailData->item_id;
			mail.mail_message = mailData->mail_message;
			mail.mail_seq = mailData->mail_seq;
			ntf.mail_datas.push_back(mail);
		}
		if(0 < ntf.mail_datas.size())
		{
			Gamnet::Network::Tcp::SendMsg(session, ntf, true);
		}
		last_update_time = now;
	}

	void Mail::Open(uint64_t mailSEQ)
	{
		auto itr = mail_datas.find(mailSEQ);
		if(mail_datas.end() == itr)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		auto mail = itr->second;
		session->queries->Execute(
			"UPDATE user_mail SET delete_yn='Y', delete_date=NOW() WHERE mail_seq=", mailSEQ
		);
		auto bag = session->GetComponent<Component::Bag>();
		bag->Insert(Item::CreateInstance(session, mail->item_id, mail->item_count));
		mail_datas.erase(itr);
	}

	void Mail::SendMail(const std::shared_ptr<UserSession>& session, const std::shared_ptr<MailData>& mailData)
	{
		auto mail = session->GetComponent<Mail>();
		if(nullptr == mail)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		session->queries->Insert("user_mail", {
			{ "user_seq", session->user_seq },
			{ "expire_date", mailData->expire_date.ToString() },
			{ "mail_message", mailData->mail_message },
			{ "item_id", mailData->item_id },
			{ "item_count", mailData->item_count }
		});
		mail->last_update_time = 0;
		session->on_commit["Mail"] = std::bind(&Mail::Load, mail);
	}
};