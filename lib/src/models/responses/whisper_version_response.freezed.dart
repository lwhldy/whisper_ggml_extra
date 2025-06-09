// dart format width=80
// coverage:ignore-file
// GENERATED CODE - DO NOT MODIFY BY HAND
// ignore_for_file: type=lint
// ignore_for_file: unused_element, deprecated_member_use, deprecated_member_use_from_same_package, use_function_type_syntax_for_parameters, unnecessary_const, avoid_init_to_null, invalid_override_different_default_values_named, prefer_expression_function_bodies, annotate_overrides, invalid_annotation_target, unnecessary_question_mark

part of 'whisper_version_response.dart';

// **************************************************************************
// FreezedGenerator
// **************************************************************************

// dart format off
T _$identity<T>(T value) => value;

/// @nodoc
mixin _$WhisperVersionResponse {
  @JsonKey(name: '@type')
  String get type;
  String get message;

  /// Create a copy of WhisperVersionResponse
  /// with the given fields replaced by the non-null parameter values.
  @JsonKey(includeFromJson: false, includeToJson: false)
  @pragma('vm:prefer-inline')
  $WhisperVersionResponseCopyWith<WhisperVersionResponse> get copyWith =>
      _$WhisperVersionResponseCopyWithImpl<WhisperVersionResponse>(
          this as WhisperVersionResponse, _$identity);

  /// Serializes this WhisperVersionResponse to a JSON map.
  Map<String, dynamic> toJson();

  @override
  bool operator ==(Object other) {
    return identical(this, other) ||
        (other.runtimeType == runtimeType &&
            other is WhisperVersionResponse &&
            (identical(other.type, type) || other.type == type) &&
            (identical(other.message, message) || other.message == message));
  }

  @JsonKey(includeFromJson: false, includeToJson: false)
  @override
  int get hashCode => Object.hash(runtimeType, type, message);

  @override
  String toString() {
    return 'WhisperVersionResponse(type: $type, message: $message)';
  }
}

/// @nodoc
abstract mixin class $WhisperVersionResponseCopyWith<$Res> {
  factory $WhisperVersionResponseCopyWith(WhisperVersionResponse value,
          $Res Function(WhisperVersionResponse) _then) =
      _$WhisperVersionResponseCopyWithImpl;
  @useResult
  $Res call({@JsonKey(name: '@type') String type, String message});
}

/// @nodoc
class _$WhisperVersionResponseCopyWithImpl<$Res>
    implements $WhisperVersionResponseCopyWith<$Res> {
  _$WhisperVersionResponseCopyWithImpl(this._self, this._then);

  final WhisperVersionResponse _self;
  final $Res Function(WhisperVersionResponse) _then;

  /// Create a copy of WhisperVersionResponse
  /// with the given fields replaced by the non-null parameter values.
  @pragma('vm:prefer-inline')
  @override
  $Res call({
    Object? type = null,
    Object? message = null,
  }) {
    return _then(_self.copyWith(
      type: null == type
          ? _self.type
          : type // ignore: cast_nullable_to_non_nullable
              as String,
      message: null == message
          ? _self.message
          : message // ignore: cast_nullable_to_non_nullable
              as String,
    ));
  }
}

/// @nodoc
@JsonSerializable()
class _WhisperVersionResponse extends WhisperVersionResponse {
  const _WhisperVersionResponse(
      {@JsonKey(name: '@type') required this.type, required this.message})
      : super._();
  factory _WhisperVersionResponse.fromJson(Map<String, dynamic> json) =>
      _$WhisperVersionResponseFromJson(json);

  @override
  @JsonKey(name: '@type')
  final String type;
  @override
  final String message;

  /// Create a copy of WhisperVersionResponse
  /// with the given fields replaced by the non-null parameter values.
  @override
  @JsonKey(includeFromJson: false, includeToJson: false)
  @pragma('vm:prefer-inline')
  _$WhisperVersionResponseCopyWith<_WhisperVersionResponse> get copyWith =>
      __$WhisperVersionResponseCopyWithImpl<_WhisperVersionResponse>(
          this, _$identity);

  @override
  Map<String, dynamic> toJson() {
    return _$WhisperVersionResponseToJson(
      this,
    );
  }

  @override
  bool operator ==(Object other) {
    return identical(this, other) ||
        (other.runtimeType == runtimeType &&
            other is _WhisperVersionResponse &&
            (identical(other.type, type) || other.type == type) &&
            (identical(other.message, message) || other.message == message));
  }

  @JsonKey(includeFromJson: false, includeToJson: false)
  @override
  int get hashCode => Object.hash(runtimeType, type, message);

  @override
  String toString() {
    return 'WhisperVersionResponse(type: $type, message: $message)';
  }
}

/// @nodoc
abstract mixin class _$WhisperVersionResponseCopyWith<$Res>
    implements $WhisperVersionResponseCopyWith<$Res> {
  factory _$WhisperVersionResponseCopyWith(_WhisperVersionResponse value,
          $Res Function(_WhisperVersionResponse) _then) =
      __$WhisperVersionResponseCopyWithImpl;
  @override
  @useResult
  $Res call({@JsonKey(name: '@type') String type, String message});
}

/// @nodoc
class __$WhisperVersionResponseCopyWithImpl<$Res>
    implements _$WhisperVersionResponseCopyWith<$Res> {
  __$WhisperVersionResponseCopyWithImpl(this._self, this._then);

  final _WhisperVersionResponse _self;
  final $Res Function(_WhisperVersionResponse) _then;

  /// Create a copy of WhisperVersionResponse
  /// with the given fields replaced by the non-null parameter values.
  @override
  @pragma('vm:prefer-inline')
  $Res call({
    Object? type = null,
    Object? message = null,
  }) {
    return _then(_WhisperVersionResponse(
      type: null == type
          ? _self.type
          : type // ignore: cast_nullable_to_non_nullable
              as String,
      message: null == message
          ? _self.message
          : message // ignore: cast_nullable_to_non_nullable
              as String,
    ));
  }
}

// dart format on
