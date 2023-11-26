#include "nanopack_message_factory.hxx"
#include "text.np.hxx"
#include "widget.np.hxx"

#include <iostream>
#include <memory>

int main() {
	std::cout << "NanoPack supports inheritance!" << std::endl;
	std::cout << "In this example, Text inherits Widget, so it inherits all "
				 "the declared fields of Widgets."
			  << std::endl;

	const std::shared_ptr<Widget> widget_ =
		std::make_shared<Text>(123, "hello world");
	const std::vector<uint8_t> serialized = widget_->data();

	int bytes_read;
	const std::shared_ptr message = std::move(
		make_nanopack_message(Text::TYPE_ID, serialized.begin(), bytes_read));

	const auto widget = std::dynamic_pointer_cast<Widget>(message);

	std::cout << "Read " << bytes_read << " bytes." << std::endl;
	std::cout << "ID of text (inherited from Widget): " << widget->id
			  << std::endl;

	const auto text = std::dynamic_pointer_cast<Text>(message);
	std::cout << "Content of text (declared): " << text->content << std::endl;
}
